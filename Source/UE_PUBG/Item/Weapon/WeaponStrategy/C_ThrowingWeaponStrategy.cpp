// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/WeaponStrategy/C_ThrowingWeaponStrategy.h"

#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"

bool AC_ThrowingWeaponStrategy::UseBKeyStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	return false;
}

bool AC_ThrowingWeaponStrategy::UseRKeyStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	AC_ThrowingWeapon* ThrowingWeapon = Cast<AC_ThrowingWeapon>(Weapon);

	if (!IsValid(ThrowingWeapon))			return false;
	if (ThrowingWeapon->GetIsCooked())		return false;
	if (!ThrowingWeapon->GetIsCharging())	return false;

	ThrowingWeapon->StartCooking();

	return true;
}

bool AC_ThrowingWeaponStrategy::UseMlb_StartedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	AC_ThrowingWeapon* ThrowingWeapon = Cast<AC_ThrowingWeapon>(Weapon);

	if (!IsValid(ThrowingWeapon)) return false;

	UAnimInstance* UserAnimInstance = WeaponUser->GetMesh()->GetAnimInstance();

	// ���� ���⸦ ������ ���� or ���⸦ ����ִ� �� (Priority ������ ���ݸ���� ����� �ȵ����� bIsOnThrowProcess true�� ���� ����)
	if (UserAnimInstance->Montage_IsPlaying(ThrowingWeapon->GetCurDrawMontage().AnimMontage) ||
		UserAnimInstance->Montage_IsPlaying(ThrowingWeapon->GetCurSheathMontage().AnimMontage))
		return false;

	if (ThrowingWeapon->GetIsOnThrowProcess()) return false;

	ThrowingWeapon->SetIsOnThrowProcess(true);
	FPriorityAnimMontage RemovePinMontage = ThrowingWeapon->GetCurRemovePinMontage();
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
