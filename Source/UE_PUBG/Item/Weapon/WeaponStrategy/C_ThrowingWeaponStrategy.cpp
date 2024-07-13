// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/WeaponStrategy/C_ThrowingWeaponStrategy.h"

#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"

bool AC_ThrowingWeaponStrategy::UseBKeyStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool AC_ThrowingWeaponStrategy::UseRKeyStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool AC_ThrowingWeaponStrategy::UseMlb_StartedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	AC_ThrowingWeapon* ThrowingWeapon = Cast<AC_ThrowingWeapon>(Weapon);

	if (!IsValid(ThrowingWeapon)) return false;

	FPriorityAnimMontage RemovePinMontage = ThrowingWeapon->GetCurRemovePinMontage();
	FPriorityAnimMontage ReadyMontage = ThrowingWeapon->GetCurThrowReadyMontage();
	FPriorityAnimMontage ThrowMontage = ThrowingWeapon->GetCurThrowMontage();


	if (
		WeaponUser->GetMesh()->GetAnimInstance()->Montage_IsPlaying(RemovePinMontage.AnimMontage) ||
		WeaponUser->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReadyMontage.AnimMontage)     ||
		WeaponUser->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ThrowMontage.AnimMontage)
		)
		return false;



	WeaponUser->PlayAnimMontage(RemovePinMontage);

	return true;
}

bool AC_ThrowingWeaponStrategy::UseMlb_OnGoingStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	AC_ThrowingWeapon* ThrowingWeapon = Cast<AC_ThrowingWeapon>(Weapon);
	if (!IsValid(ThrowingWeapon)) return false;

	ThrowingWeapon->SetIsCharging(true);

	return true;
}

bool AC_ThrowingWeaponStrategy::UseMlb_CompletedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	AC_ThrowingWeapon* ThrowingWeapon = Cast<AC_ThrowingWeapon>(Weapon);
	if (!IsValid(ThrowingWeapon)) return false;

	ThrowingWeapon->SetIsCharging(false);

	return true;
}

bool AC_ThrowingWeaponStrategy::UseMrb_StartedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool AC_ThrowingWeaponStrategy::UseMrb_OnGoingStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool AC_ThrowingWeaponStrategy::UseMrb_CompletedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}
