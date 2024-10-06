// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Weapon/WeaponStrategy/C_GunStrategy.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
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
#include "UMG.h"
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
	//UC_Util::Print("Mlb Clicked");
	AC_Gun* CurWeapon = Cast<AC_Gun>(Weapon);
	MlbPressTimeCount = 0;
	
	return CurWeapon->FireBullet();
}

bool AC_GunStrategy::UseMlb_OnGoingStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	AC_Gun* CurWeapon = Cast<AC_Gun>(Weapon);
	//총알 연발
	//TODO: 총알의 연사율 받아와서 적용 + 총알 발사 모드에 따라 작동못하게 하기
	MlbPressTimeCount += WeaponUser->GetWorld()->GetDeltaSeconds();
	if (MlbPressTimeCount > CurWeapon->GetBulletRPM())
	{
		MlbPressTimeCount -= CurWeapon->GetBulletRPM();
		CurWeapon->FireBullet();
	}
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
	UC_Util::Print("Mrb Clicked");



	return false;
}

bool AC_GunStrategy::UseMrb_OnGoingStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	AC_Gun* CurWeapon = Cast<AC_Gun>(Weapon);

	if (WeaponUser->GetMesh()->GetAnimInstance()->Montage_IsPlaying(Weapon->GetCurDrawMontage().AnimMontage)) return false;
	if (WeaponUser->GetMesh()->GetAnimInstance()->Montage_IsPlaying(Weapon->GetCurSheathMontage().AnimMontage)) return false;
	MrbPressTimeCount += WeaponUser->GetWorld()->GetDeltaSeconds();

	if (MrbPressTimeCount >= 0.2)
	{
		CurWeapon->SetIsAimPress(false);
		//UC_Util::Print("False");
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
	if (!IsValid(CurUser))   return false;
	if (!IsValid(CurWeapon)) return false;

	if (!CurWeapon->GetIsAimPress() && MrbPressTimeCount < 0.2)
	{
		CurWeapon->SetAimingDown();
		return true;

	}
	if (CurWeapon->GetIsAimPress())
	{
		
		CurWeapon->BackToMainCamera();

		return true;

	}
	CurWeapon->BackToMainCamera();
	CurWeapon->SetIsAimPress(false);

	//CurUser->BackToMainCamera();
	return true;
}
