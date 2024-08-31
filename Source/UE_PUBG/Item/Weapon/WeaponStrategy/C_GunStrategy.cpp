// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Weapon/WeaponStrategy/C_GunStrategy.h"
#include "GameFramework/Actor.h"
#include "Character/C_BasicCharacter.h"
#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Weapon/MeleeWeapon/C_MeleeWeapon.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimMontage.h"
#include "Character/C_BasicCharacter.h"
#include "Utility/C_Util.h"

#include "Character/C_Player.h"


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
	AC_Gun* CurWeapon = Cast<AC_Gun>(Weapon);

	CurWeapon->SetAimingPress();



	return false;
}

bool AC_GunStrategy::UseMrb_OnGoingStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	AC_Gun* CurWeapon = Cast<AC_Gun>(Weapon);

	if (WeaponUser->GetMesh()->GetAnimInstance()->Montage_IsPlaying(Weapon->GetCurDrawMontage().AnimMontage)) return false;
	if (WeaponUser->GetMesh()->GetAnimInstance()->Montage_IsPlaying(Weapon->GetCurSheathMontage().AnimMontage)) return false;
	MrbPressTimeCount += WeaponUser->GetWorld()->GetDeltaSeconds();
	//UC_Util::Print(MrbPressTimeCount);
	//if (CurWeapon->GetIsAimPress())
	//{
	//	//FString TheFloatStr = "AimingOn";
	//	//WeaponUser->bUseControllerRotationYaw = true;
	//	//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);
	//	return false;
	//}
	if (bIsAimDownSight)
	{
		//WeaponUser->bUseControllerRotationYaw = true;

	}
	//FString TheFloatStr = FString::SanitizeFloat(MrbPressTimeCount);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);
	if (MrbPressTimeCount >= 0.2)
	{
		bIsAimDownSight = false;
		return true;
	}



	return true;
}

bool AC_GunStrategy::UseMrb_CompletedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	//MrbPressTimeCount = 0;
	if (WeaponUser->GetMesh()->GetAnimInstance()->Montage_IsPlaying(Weapon->GetCurDrawMontage().AnimMontage)) return false;
	if (WeaponUser->GetMesh()->GetAnimInstance()->Montage_IsPlaying(Weapon->GetCurSheathMontage().AnimMontage)) return false;
	AC_Player* CurUser = Cast<AC_Player>(WeaponUser);
	AC_Gun* CurWeapon = Cast<AC_Gun>(Weapon);
	
	if (!bIsAimDownSight && MrbPressTimeCount < 0.2)
	{
		CurWeapon->SetAimingDown();
		bIsAimDownSight = true;
		return true;

	}
	if (bIsAimDownSight)
	{
		CurWeapon->BackToMainCamera();

		bIsAimDownSight = false;
		return true;

	}
	CurWeapon->BackToMainCamera();
	CurWeapon->SetIsAimPress(false);

	CurUser->BackToMainCamera();
	return true;
}
