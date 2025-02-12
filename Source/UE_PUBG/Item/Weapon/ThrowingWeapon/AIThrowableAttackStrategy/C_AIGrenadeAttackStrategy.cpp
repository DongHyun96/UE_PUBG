// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/ThrowingWeapon/AIThrowableAttackStrategy/C_AIGrenadeAttackStrategy.h"

#include "Utility/C_Util.h"

bool UC_AIGrenadeAttackStrategy::ExecuteAIAttack(AC_ThrowingWeapon* ThrowingWeapon, AC_BasicCharacter* InTargetCharacter)
{
	UC_Util::Print("Try Execute Grenade AI Attack Strategy", FColor::Red, 10.f);
	return false;
}

bool UC_AIGrenadeAttackStrategy::ExecuteAIAttackTickTask(AC_ThrowingWeapon* ThrowingWeapon, AC_BasicCharacter* InTargetCharacter, const float& DeltaTime)
{
	UC_Util::Print("Try Execute Grenade AI Attack TickTask Strategy", FColor::Red, 10.f);
	return false;
}
