// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/ThrowingWeapon/AIThrowableAttackStrategy/C_AIFlashBangAttackStrategy.h"

#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/C_Util.h"

const float UC_AIFlashBangAttackStrategy::TIME_TO_THROW = 2.f;

bool UC_AIFlashBangAttackStrategy::ExecuteAIAttack(AC_ThrowingWeapon* ThrowingWeapon, AC_BasicCharacter* InTargetCharacter)
{
	// TODO : TargetCharacter 위치로부터 적정 Offset주어 던질 Projectile destination 지정
	// SuggestTossVelocity 통해 Velocity 정하기 -> 이건 던지기 직전에만 계산을 해주면 됨
	// 쿠킹 없이 바로 던지기

	if (!ThrowingWeapon->ExecuteMlb_Started()) return false;
	return true;
}

bool UC_AIFlashBangAttackStrategy::ExecuteAIAttackTickTask(AC_ThrowingWeapon* ThrowingWeapon, AC_BasicCharacter* InTargetCharacter, const float& DeltaTime)
{
	// TODO : TargetCharacter 방면으로 계속 회전시키기

	ThrowTimer += DeltaTime;

	if (ThrowTimer < TIME_TO_THROW)
	{
		ThrowingWeapon->ExecuteMlb_OnGoing();

		// TargetCharacter 방면으로 회전 주기
		AC_BasicCharacter* Enemy = ThrowingWeapon->GetOwnerCharacter();
		
		
		return true;
	}

	ThrowingWeapon->ExecuteMlb_Completed();
	return false;
}

bool UC_AIFlashBangAttackStrategy::UpdateProjectileLaunchValues(class AC_ThrowingWeapon* ThrowingWeapon, class AC_BasicCharacter* TargetCharacter)
{
	if (!IsValid(ThrowingWeapon))  return false;
	if (!IsValid(TargetCharacter)) return false;
	
	FVector ThrowStartLocation = ThrowingWeapon->GetPredictedThrowStartLocation();
	ThrowingWeapon->SetProjectileStartLocation(ThrowStartLocation); // StartLocation 세팅	

	FVector LaunchVelocity{};
	
	bool HasVelocityInited = UGameplayStatics::SuggestProjectileVelocity
	(
		ThrowingWeapon->GetWorld(),
		LaunchVelocity,
		ThrowStartLocation,
		GetRandomNearByPositionFromTargetCharacter(TargetCharacter->GetActorLocation()),
		ThrowingWeapon->GetSpeed(),
		false,
		AC_ThrowingWeapon::GetProjectileRadius(),
		0,
		ESuggestProjVelocityTraceOption::OnlyTraceWhileAscending
	);

	if (!HasVelocityInited) return false;

	ThrowingWeapon->SetProjectileLaunchVelocity(LaunchVelocity);
	return true;	
}

bool UC_AIFlashBangAttackStrategy::IsPossibleDistanceToThrow(const FVector& ThrowingWeaponLocation, const FVector& TargetCharacterLocation)
{
	static const float DISTANCE_LIMIT = 3500.f; // 35m Distance limit
	return FVector::Distance(ThrowingWeaponLocation, TargetCharacterLocation) <= DISTANCE_LIMIT;
}

FVector UC_AIFlashBangAttackStrategy::GetRandomNearByPositionFromTargetCharacter(const FVector& TargetCharacterLocation)
{
	// -2m ~ 2m 이내로 Random한 position 잡기 (높이는 TargetCharacter높이로 지정)
	static const float XYOFFSET_LIMIT = 200.f;
	
	float X = FMath::RandRange(TargetCharacterLocation.X - XYOFFSET_LIMIT, TargetCharacterLocation.X + XYOFFSET_LIMIT);
	float Y = FMath::RandRange(TargetCharacterLocation.Y - XYOFFSET_LIMIT, TargetCharacterLocation.Y + XYOFFSET_LIMIT);
	return FVector(X, Y, TargetCharacterLocation.Z);
}

