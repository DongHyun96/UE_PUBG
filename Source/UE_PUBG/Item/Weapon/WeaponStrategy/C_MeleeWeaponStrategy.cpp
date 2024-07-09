// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/WeaponStrategy/C_MeleeWeaponStrategy.h"

#include "Character/C_BasicCharacter.h"
#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/MeleeWeapon/C_MeleeWeapon.h"

bool UC_MeleeWeaponStrategy::UseBKeyStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool UC_MeleeWeaponStrategy::UseRKeyStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool UC_MeleeWeaponStrategy::UseMlb_StartedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	AC_MeleeWeapon* MeleeWeapon = Cast<AC_MeleeWeapon>(Weapon);

	if (!IsValid(MeleeWeapon)) return false;

	FPriorityAnimMontage AttackMontage = MeleeWeapon->GetAttackMontage();

	if (WeaponUser->GetMesh()->GetAnimInstance()->Montage_IsPlaying(AttackMontage.AnimMontage))
		return false;

	WeaponUser->PlayAnimMontage(AttackMontage);

	return false;
}

bool UC_MeleeWeaponStrategy::UseMlb_OnGoingStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool UC_MeleeWeaponStrategy::UseMlb_CompletedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool UC_MeleeWeaponStrategy::UseMrb_StartedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool UC_MeleeWeaponStrategy::UseMrb_OnGoingStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool UC_MeleeWeaponStrategy::UseMrb_CompletedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}
