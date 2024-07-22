// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Weapon/WeaponStrategy/C_GunStrategy.h"
#include "GameFramework/Actor.h"
#include "Character/C_BasicCharacter.h"
#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/MeleeWeapon/C_MeleeWeapon.h"

bool AC_GunStrategy::UseBKeyStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool AC_GunStrategy::UseRKeyStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool AC_GunStrategy::UseMlb_StartedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool AC_GunStrategy::UseMlb_OnGoingStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool AC_GunStrategy::UseMlb_CompletedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool AC_GunStrategy::UseMrb_StartedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	MrbPressTimeCount = 0;
	return false;
}

bool AC_GunStrategy::UseMrb_OnGoingStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	MrbPressTimeCount += WeaponUser->GetWorld()->GetDeltaSeconds();
	FString TheFloatStr = FString::SanitizeFloat(MrbPressTimeCount);
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);
	if (MrbPressTimeCount >= 0.2)
		WeaponUser->SetToAimKeyPress();
	return false;
}

bool AC_GunStrategy::UseMrb_CompletedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	//MrbPressTimeCount = 0;
	WeaponUser->BackToMainCamera();
	return false;
}
