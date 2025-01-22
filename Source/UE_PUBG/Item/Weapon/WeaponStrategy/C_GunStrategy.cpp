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
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_InvenSystem.h"

#include "Character/Component/C_AttachableItemMeshComponent.h"
#include "Item/Weapon/Gun/C_SR.h"



bool AC_GunStrategy::UseBKeyStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	AC_Player* CurPlayer = Cast<AC_Player>(WeaponUser);
	if (!IsValid(CurPlayer)) return false;
	if (CurPlayer->GetInvenSystem()->GetInvenUI()->GetIsPanelOpened()) return false; //UI가 열려 있을때 작동 금지.

	AC_Gun* CurWeapon = Cast<AC_Gun>(Weapon);
	//UC_Util::Print("Change Weapon Mode");
	CurWeapon->ChangeCurShootingMode();	
	return false;
}

bool AC_GunStrategy::UseRKeyStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	//if (WeaponUser->GetIsHoldDirection()) return false;
	AC_Player* CurPlayer = Cast<AC_Player>(WeaponUser);
	if (!IsValid(CurPlayer)) return false;
	if (CurPlayer->GetInvenSystem()->GetInvenUI()->GetIsPanelOpened()) return false; //UI가 열려 있을때 작동 금지.

	AC_Gun* CurWeapon = Cast<AC_Gun>(Weapon);
	//if (CurWeapon->GetIsPartAttached(EPartsName::GRIP))
	//{
	//	WeaponUser->GetAttachmentMeshComponent()->DetachFromGun(CurWeapon->GetGunMesh(), EPartsName::GRIP, EAttachmentNames::VERTGRIP);
	//	WeaponUser->GetAttachmentMeshComponent()->DetachFromGun(CurWeapon->GetGunMesh(), EPartsName::MUZZLE, EAttachmentNames::COMPENSATOR);
	//	return true;
	//}
	//WeaponUser->GetAttachmentMeshComponent()->AttachToGun(CurWeapon->GetGunMesh(), EPartsName::GRIP, EAttachmentNames::VERTGRIP);
	//WeaponUser->GetAttachmentMeshComponent()->AttachToGun(CurWeapon->GetGunMesh(), EPartsName::MUZZLE, EAttachmentNames::COMPENSATOR);

	AC_SR* CurrentSR = Cast<AC_SR>(CurWeapon);
	if (IsValid(CurrentSR))
		CurrentSR->SetIsReloadingSR(true);
	CurWeapon->ExecuteReloadMontage();
	return false;
}

bool AC_GunStrategy::UseMlb_StartedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	//UC_Util::Print("Mlb Clicked");
	AC_Player* CurPlayer = Cast<AC_Player>(WeaponUser);
	if (!IsValid(CurPlayer)) return false;
	if (CurPlayer->GetInvenSystem()->GetInvenUI()->GetIsPanelOpened()) return false; //UI가 열려 있을때 작동 금지.

	if (WeaponUser->GetIsHoldDirection()) return false;
	if (!WeaponUser->GetCanFireBullet()) return false;

	AC_Gun* CurWeapon = Cast<AC_Gun>(Weapon);
	
	if (CurWeapon->GetIsPlayingMontagesOfAny() || CurWeapon->GetCanGunAction()) return false;
	MlbPressTimeCount = 0;
	if (CurWeapon->GetCurBulletCount() < CurWeapon->GetMaxBulletCount() && CurWeapon->GetCurBulletCount() > 0)
	{
		if(CurWeapon->GetCurrentShootingMode() == EShootingMode::SINGLE_SHOT && !CurPlayer->GetIsAimDown())
			CurWeapon->ExecuteReloadMontage();
	}
	else if (CurWeapon->GetCurBulletCount() == 0)
	{

		CurWeapon->ExecuteReloadMontage();
		return false;
	}

	return CurWeapon->FireBullet();
}

bool AC_GunStrategy::UseMlb_OnGoingStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	if (WeaponUser->GetIsHoldDirection()) return false;
	if (!WeaponUser->GetCanFireBullet()) return false;
	AC_Player* CurPlayer = Cast<AC_Player>(WeaponUser);
	if (CurPlayer->GetInvenSystem()->GetInvenUI()->GetIsPanelOpened()) return false; //UI가 열려 있을때 작동 금지.

	AC_Gun* CurWeapon = Cast<AC_Gun>(Weapon);
	if (CurWeapon->GetIsPlayingMontagesOfAny() || CurWeapon->GetCanGunAction()) return false;


	//총알 연발
	//TODO: 총알의 연사율 받아와서 적용 + 총알 발사 모드에 따라 작동못하게 하기
	if(CurWeapon->GetCurrentShootingMode() != EShootingMode::FULL_AUTO) return false;
	MlbPressTimeCount += WeaponUser->GetWorld()->GetDeltaSeconds();
	if (MlbPressTimeCount > CurWeapon->GetBulletRPM())
	{
		MlbPressTimeCount -= CurWeapon->GetBulletRPM();
		if (CurWeapon->GetCurBulletCount() == 0)
		{
			CurWeapon->ExecuteReloadMontage();
			return false;
		}
		CurWeapon->FireBullet();
	}
	return true;
}

bool AC_GunStrategy::UseMlb_CompletedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	AC_Player* CurPlayer = Cast<AC_Player>(WeaponUser);
	if (!IsValid(CurPlayer)) return false;
	if (CurPlayer->GetInvenSystem()->GetInvenUI()->GetIsPanelOpened()) return false; //UI가 열려 있을때 작동 금지.
	if (WeaponUser->GetIsHoldDirection()) return false;
	if (!WeaponUser->GetCanFireBullet()) return false;

	AC_Gun* CurWeapon = Cast<AC_Gun>(Weapon);
	if (CurPlayer->GetIsAimDown() && CurWeapon->GetCurrentShootingMode() == EShootingMode::SINGLE_SHOT && CurWeapon->GetCurBulletCount() >= 0)
		CurWeapon->ExecuteReloadMontage();
	if (CurWeapon->GetIsPlayingMontagesOfAny() || CurWeapon->GetCanGunAction()) return false;


	return false;
}

bool AC_GunStrategy::UseMrb_StartedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	AC_Player* CurPlayer = Cast<AC_Player>(WeaponUser);
	if (!IsValid(CurPlayer)) return false;
	if (CurPlayer->GetInvenSystem()->GetInvenUI()->GetIsPanelOpened()) return false; //UI가 열려 있을때 작동 금지.
	if (WeaponUser->GetIsHoldDirection()) return false;
	//if (!WeaponUser->GetCanFireBullet()) return false;

	MrbPressTimeCount = 0;
	AC_Gun* CurWeapon = Cast<AC_Gun>(Weapon);
	if (CurWeapon->GetIsPlayingMontagesOfAny() || CurWeapon->GetCanGunAction()) return false;


	if (WeaponUser->GetNextSpeed() != 0 && WeaponUser->GetPoseState() == EPoseState::CRAWL)
	{
		return false;
	}
	CurWeapon->SetAimingPress();
	//UC_Util::Print("Mrb Clicked");



	return true;
}

bool AC_GunStrategy::UseMrb_OnGoingStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	AC_Player* CurPlayer = Cast<AC_Player>(WeaponUser);
	if (!IsValid(CurPlayer)) return false;
	if (CurPlayer->GetInvenSystem()->GetInvenUI()->GetIsPanelOpened()) return false; //UI가 열려 있을때 작동 금지.
	if (WeaponUser->GetIsHoldDirection()) return false;
	//if (!WeaponUser->GetCanFireBullet()) return false;

	AC_Gun* CurWeapon = Cast<AC_Gun>(Weapon);
	if (CurWeapon->GetIsPlayingMontagesOfAny() || CurWeapon->GetCanGunAction()) return false;


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
	AC_Player* CurPlayer = Cast<AC_Player>(WeaponUser);
	if (!IsValid(CurPlayer)) return false;
	if (CurPlayer->GetInvenSystem()->GetInvenUI()->GetIsPanelOpened()) return false; //UI가 열려 있을때 작동 금지.
	if (WeaponUser->GetIsHoldDirection()) return false;
	//if (!WeaponUser->GetCanFireBullet()) return false;

	//MrbPressTimeCount = 0;
	if (WeaponUser->GetMesh()->GetAnimInstance()->Montage_IsPlaying(Weapon->GetCurDrawMontage().AnimMontage)) return false;
	if (WeaponUser->GetMesh()->GetAnimInstance()->Montage_IsPlaying(Weapon->GetCurSheathMontage().AnimMontage)) return false;
	AC_Player* CurUser = Cast<AC_Player>(WeaponUser);
	AC_Gun* CurWeapon = Cast<AC_Gun>(Weapon);
	if (CurWeapon->GetIsPlayingMontagesOfAny() || CurWeapon->GetCanGunAction()) return false;


	if (!IsValid(CurUser))   return false;
	if (!IsValid(CurWeapon)) return false;

	if (!CurWeapon->GetIsAimPress() && MrbPressTimeCount < 0.2)
	{
		if (WeaponUser->GetNextSpeed() != 0 && WeaponUser->GetPoseState() == EPoseState::CRAWL)
		{
			return false;
		}
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
