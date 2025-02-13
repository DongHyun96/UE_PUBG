// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/ThrowingWeapon/AIThrowableAttackStrategy/C_AIThrowingStrategy.h"

#include "Character/Component/C_EquippedComponent.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

const float UC_AIThrowingStrategy::TIME_TO_THROW = 1.5f;

bool UC_AIThrowingStrategy::ExecuteAIAttack(AC_ThrowingWeapon* ThrowingWeapon, AC_BasicCharacter* InTargetCharacter)
{
	if (!ThrowingWeapon->ExecuteMlb_Started()) return false;
	return true;
}

bool UC_AIThrowingStrategy::ExecuteAIAttackTickTask(AC_ThrowingWeapon* ThrowingWeapon, AC_BasicCharacter* InTargetCharacter, const float& DeltaTime)
{
	ThrowTimer += DeltaTime;

	// TargetCharacter가 사라졌거나, Target까지의 거리가 너무 멀어졌을 때
	if (!IsValid(InTargetCharacter) ||
		!IsPossibleDistanceToThrow(ThrowingWeapon->GetActorLocation(), InTargetCharacter->GetActorLocation()))
	{
		// 던지기 취소(HandState None으로 돌아가기)
		ThrowingWeapon->GetOwnerCharacter()->GetEquippedComponent()->ChangeCurWeapon(EWeaponSlot::NONE);
		return false;
	}

	if (ThrowTimer < TIME_TO_THROW)
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

	// 쿠킹 없이 바로 던지기
	
	// 던질 시간
	if (!UpdateProjectileLaunchValues(ThrowingWeapon, InTargetCharacter->GetActorLocation()))
	{
		// LaunchValues를 초기화 할 수 없을 때(ex) 던지는 위로 향하는 궤적에 물체가 있을 때)
		ThrowingWeapon->GetOwnerCharacter()->GetEquippedComponent()->ChangeCurWeapon(EWeaponSlot::NONE);
		return false;
	}

	// 던지기
	ThrowingWeapon->ExecuteMlb_Completed();
	return false;
}

bool UC_AIThrowingStrategy::UpdateProjectileLaunchValues(AC_ThrowingWeapon* ThrowingWeapon, const FVector& TargetCharacterLocation)
{
	if (!IsValid(ThrowingWeapon))  return false;
	
	FVector ThrowStartLocation = ThrowingWeapon->GetPredictedThrowStartLocation();
	ThrowingWeapon->SetProjectileStartLocation(ThrowStartLocation); // StartLocation 세팅	

	FVector LaunchVelocity{};
	
	bool HasVelocityInited = UGameplayStatics::SuggestProjectileVelocity
	(
		ThrowingWeapon->GetWorld(),
		LaunchVelocity,
		ThrowStartLocation,
		GetRandomNearByPositionFromTargetCharacter(TargetCharacterLocation),
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

FVector UC_AIThrowingStrategy::GetRandomNearByPositionFromTargetCharacter(const FVector& TargetCharacterLocation)
{
	// -2m ~ 2m 이내로 Random한 position 잡기 (높이는 TargetCharacter높이로 지정)
	static const float XYOFFSET_LIMIT = 200.f;
	
	float X = FMath::RandRange(TargetCharacterLocation.X - XYOFFSET_LIMIT, TargetCharacterLocation.X + XYOFFSET_LIMIT);
	float Y = FMath::RandRange(TargetCharacterLocation.Y - XYOFFSET_LIMIT, TargetCharacterLocation.Y + XYOFFSET_LIMIT);
	return FVector(X, Y, TargetCharacterLocation.Z);
}
