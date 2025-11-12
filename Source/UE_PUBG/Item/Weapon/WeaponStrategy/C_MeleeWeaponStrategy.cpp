// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/WeaponStrategy/C_MeleeWeaponStrategy.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Character/Component/C_InvenSystem.h"
#include "Character/Component/C_ParkourComponent.h"
#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/MeleeWeapon/C_MeleeWeapon.h"
#include "Serialization/AsyncPackageLoader.h"
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

	if (AC_Player* Player = Cast<AC_Player>(WeaponUser))
		if (Player->GetInvenSystem()->GetInvenUI()->GetIsPanelOpened()) return false; // UI가 열려 있을 때 작동 금지


	// 파쿠르 처리 중 작동 금지
	if (WeaponUser->GetMesh()->GetSkeletalMeshAsset() == WeaponUser->GetParkourComponent()->GetRootedSkeletalMesh())
		return false;

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


