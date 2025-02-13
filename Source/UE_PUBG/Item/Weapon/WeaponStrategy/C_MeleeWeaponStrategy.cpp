// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/WeaponStrategy/C_MeleeWeaponStrategy.h"

#include "Character/C_BasicCharacter.h"
#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/MeleeWeapon/C_MeleeWeapon.h"
#include "Utility/C_Util.h"

bool AC_MeleeWeaponStrategy::UseBKeyStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool AC_MeleeWeaponStrategy::UseRKeyStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool AC_MeleeWeaponStrategy::UseMlb_StartedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	if (WeaponUser->GetPoseState() == EPoseState::CRAWL) return false;

	AC_MeleeWeapon* MeleeWeapon = Cast<AC_MeleeWeapon>(Weapon);

	if (!IsValid(MeleeWeapon)) return false;

	FPriorityAnimMontage AttackMontage = MeleeWeapon->GetAttackMontage();

	if (WeaponUser->GetMesh()->GetAnimInstance()->Montage_IsPlaying(AttackMontage.AnimMontage))
		return false;

	WeaponUser->PlayAnimMontage(AttackMontage);

	return true;
}

bool AC_MeleeWeaponStrategy::UseMlb_OnGoingStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return UseMlb_StartedStrategy(WeaponUser, Weapon);
}

bool AC_MeleeWeaponStrategy::UseMlb_CompletedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool AC_MeleeWeaponStrategy::UseMrb_StartedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool AC_MeleeWeaponStrategy::UseMrb_OnGoingStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool AC_MeleeWeaponStrategy::UseMrb_CompletedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}


