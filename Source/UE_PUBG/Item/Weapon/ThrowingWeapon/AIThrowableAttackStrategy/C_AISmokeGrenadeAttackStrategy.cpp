// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/ThrowingWeapon/AIThrowableAttackStrategy/C_AISmokeGrenadeAttackStrategy.h"

#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Utility/C_Util.h"

const float UC_AISmokeGrenadeAttackStrategy::TIME_TO_COOK_AND_RELEASE = 5.f;

bool UC_AISmokeGrenadeAttackStrategy::ExecuteAIAttack(AC_ThrowingWeapon* ThrowingWeapon, AC_BasicCharacter* InTargetCharacter)
{
	//핀 뽑기 실행 시도
	if (!ThrowingWeapon->ExecuteMlb_Started()) return false;
	
	//ThrowingWeapon->StartCooking();
	//ThrowingWeapon->ReleaseOnGround();

	CookingStartTimer = 0.f;
	
	return true;
}

bool UC_AISmokeGrenadeAttackStrategy::ExecuteAIAttackTickTask(AC_ThrowingWeapon* ThrowingWeapon, AC_BasicCharacter* InTargetCharacter, const float& DeltaTime)
{
	CookingStartTimer += DeltaTime;

	if (CookingStartTimer < TIME_TO_COOK_AND_RELEASE)
	{
		ThrowingWeapon->ExecuteMlb_OnGoing();
		return true;
	}
	
	ThrowingWeapon->StartCooking();
	ThrowingWeapon->ReleaseOnGround();
	return false;
}
