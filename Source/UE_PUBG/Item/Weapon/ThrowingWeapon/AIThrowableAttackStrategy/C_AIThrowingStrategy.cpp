// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/ThrowingWeapon/AIThrowableAttackStrategy/C_AIThrowingStrategy.h"

#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"
#include "Character/C_Enemy.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_SmokeEnteredChecker.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"

const float UC_AIThrowingStrategy::TIME_TO_THROW = 1.5f;

bool UC_AIThrowingStrategy::ExecuteAIAttack(AC_ThrowingWeapon* ThrowingWeapon, AC_BasicCharacter* InTargetCharacter)
{
	if (!ThrowingWeapon->ExecuteMlb_Started()) return false;
	ThrowTimers.Add(ThrowingWeapon, 0.f);
	return true;
}

bool UC_AIThrowingStrategy::ExecuteAIAttackTickTask(AC_ThrowingWeapon* ThrowingWeapon, AC_BasicCharacter* InTargetCharacter, const float& DeltaTime)
{
	ThrowTimers[ThrowingWeapon] += DeltaTime;

	// TargetCharacter가 사라졌거나, Target까지의 거리가 너무 멀어졌을 때
	if (!IsValid(InTargetCharacter) || !IsPossibleDistanceToThrow(ThrowingWeapon->GetActorLocation(), InTargetCharacter->GetActorLocation()))
	{
		// 던지기 취소(HandState None으로 돌아가기)
		ThrowingWeapon->GetOwnerCharacter()->GetEquippedComponent()->ToggleArmed();
		
		if (AC_Enemy* WeaponOwnerEnemy = Cast<AC_Enemy>(ThrowingWeapon->GetOwnerCharacter()))
			WeaponOwnerEnemy->GetController<AC_EnemyAIController>()->GetBehaviorComponent()->SetAfterAttackTaskWaitTime(3.f);			
		else UC_Util::Print("From AIThrowingStrategy::ExecuteAIAttackTickTask : WeaponOwnerEnemy casting failed!", FColor::Red, 10.f);
		
		return false;
	}
	
	// 이미 TargetCharacter가 사망 처리 되었을 때
	if (InTargetCharacter->GetMainState() == EMainState::DEAD)
	{
		// 던지기 취소(HandState None으로 돌아가기)
		ThrowingWeapon->GetOwnerCharacter()->GetEquippedComponent()->ToggleArmed();

		if (AC_Enemy* WeaponOwnerEnemy = Cast<AC_Enemy>(ThrowingWeapon->GetOwnerCharacter()))
			WeaponOwnerEnemy->GetController<AC_EnemyAIController>()->GetBehaviorComponent()->SetAfterAttackTaskWaitTime(3.f);			
		else UC_Util::Print("From AIThrowingStrategy::ExecuteAIAttackTickTask : WeaponOwnerEnemy casting failed!", FColor::Red, 10.f);
		
		return false;
	}

	if (ThrowTimers[ThrowingWeapon] < TIME_TO_THROW)
	{
		ThrowingWeapon->ExecuteMlb_OnGoing();

		// TargetCharacter 방면으로 회전 주기
		AC_BasicCharacter* Enemy = ThrowingWeapon->GetOwnerCharacter();
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation
		(
		Enemy->GetActorLocation(),
		InTargetCharacter->GetActorLocation()
		);

		LookAtRotation.Pitch = 0.f;
		LookAtRotation.Roll = 0.f;
		Enemy->SetActorRotation(LookAtRotation);
		
		return true;
	}

	
	// 던질 시간, 쿠킹 없이 바로 던지기

	FVector TargetThrowLocation{};
	bool bUseRandomNearByThrowTargetLocation{};
	
	if (InTargetCharacter->GetSmokeEnteredChecker()->IsCurrentlyInSmokeArea())
	{
		InTargetCharacter->GetSmokeEnteredChecker()->GetRandomLocationInSmokeArea(TargetThrowLocation);
		bUseRandomNearByThrowTargetLocation = false;
	}
	else
	{
		TargetThrowLocation = InTargetCharacter->GetActorLocation();
		bUseRandomNearByThrowTargetLocation = true;
	}

	ThrowTimers[ThrowingWeapon] = 0.f;

	// 던지기 실패 또는 던지기 성공 모두 AttackTask 이 후, 2초 기다리기(무기 집어넣는 시간 및 무기 던지는 모션 기다리기 시간 기다려주기)
	if (AC_Enemy* WeaponOwnerEnemy = Cast<AC_Enemy>(ThrowingWeapon->GetOwnerCharacter()))
		WeaponOwnerEnemy->GetController<AC_EnemyAIController>()->GetBehaviorComponent()->SetAfterAttackTaskWaitTime(4.f);			
	else UC_Util::Print("From AIThrowingStrategy::ExecuteAIAttackTickTask : WeaponOwnerEnemy casting failed!", FColor::Red, 10.f);

	if (UpdateProjectileLaunchValues(ThrowingWeapon, TargetThrowLocation, bUseRandomNearByThrowTargetLocation))
	{
		// 던지기
		ThrowingWeapon->ExecuteMlb_Completed();
		return false;
	}
	
	// LaunchValues를 초기화 할 수 없을 때(ex) 던지는 위로 향하는 궤적에 물체가 있을 때)
	UWorld* World = ThrowingWeapon->GetWorld();
	UC_Util::Print("On AI ThrowingStrategy : Cannot init LaunchValues! terminating TickTask strategy!", UC_GameSceneManager::GetInstance(World)->GetTickRandomColor(), 10.f);
	
	bool bToggleArmedSucceeded = ThrowingWeapon->GetOwnerCharacter()->GetEquippedComponent()->ToggleArmed();
	
	FString Str = (bToggleArmedSucceeded) ? "Toggle Armed Succeeded!" : "Toggle Armed Failed!";
	UC_Util::Print(Str, UC_GameSceneManager::GetInstance(World)->GetTickRandomColor(), 10.f);
	
	return false;
}

bool UC_AIThrowingStrategy::UpdateProjectileLaunchValues(AC_ThrowingWeapon* ThrowingWeapon, const FVector& TargetThrowLocation, const bool& bUseRandomNearByThrowTargetLocation)
{
	if (!IsValid(ThrowingWeapon)) return false;
	
	FVector ThrowStartLocation = ThrowingWeapon->GetPredictedThrowStartLocation();
	ThrowingWeapon->SetProjectileStartLocation(ThrowStartLocation); // StartLocation 세팅

	FVector LaunchVelocity{};
	FVector EndTargetLocation = bUseRandomNearByThrowTargetLocation ? GetRandomNearByPositionFromTargetCharacter(TargetThrowLocation) : TargetThrowLocation; 
	
	bool HasVelocityInited = UGameplayStatics::SuggestProjectileVelocity
	(
		ThrowingWeapon->GetWorld(),
		LaunchVelocity,
		ThrowStartLocation,
		EndTargetLocation,
		ThrowingWeapon->GetSpeed(),
		false,
		AC_ThrowingWeapon::GetProjectileRadius(),
		0,
		ESuggestProjVelocityTraceOption::OnlyTraceWhileAscending
	//	FCollisionResponseParams::DefaultResponseParam,
	//	{},
	//	true
	);

	if (!HasVelocityInited) return false;

	ThrowingWeapon->SetProjectileLaunchVelocity(LaunchVelocity);
	return true;
}

bool UC_AIThrowingStrategy::IsPossibleDistanceToThrow(const FVector& ThrowingWeaponLocation, const FVector& TargetCharacterLocation)
{
	static const float DISTANCE_LIMIT = 2500.f; // 25m Distance limit
	return FVector::Distance(ThrowingWeaponLocation, TargetCharacterLocation) <= DISTANCE_LIMIT;
}

FVector UC_AIThrowingStrategy::GetRandomNearByPositionFromTargetCharacter(const FVector& TargetThrowLocation)
{
	// -2m ~ 2m 이내로 Random한 position 잡기 (높이는 TargetCharacter높이로 지정)
	static const float XYOFFSET_LIMIT = 200.f;
	
	float X = FMath::RandRange(TargetThrowLocation.X - XYOFFSET_LIMIT, TargetThrowLocation.X + XYOFFSET_LIMIT);
	float Y = FMath::RandRange(TargetThrowLocation.Y - XYOFFSET_LIMIT, TargetThrowLocation.Y + XYOFFSET_LIMIT);
	return FVector(X, Y, TargetThrowLocation.Z);
}
