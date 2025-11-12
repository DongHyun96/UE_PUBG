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
#include "HUD/C_HUDWidget.h"
#include "HUD/C_InformWidget.h"
#include "Item/Weapon/Gun/C_SR.h"



bool AC_GunStrategy::UseBKeyStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	// Shooting Mode 바꾸기
	if (WeaponUser->GetHandState() !=EHandState::WEAPON_GUN) return false;
	
	AC_Player* CurPlayer = Cast<AC_Player>(WeaponUser);
	if (!IsValid(CurPlayer)) return false;
	if (CurPlayer->GetInvenSystem()->GetInvenUI()->GetIsPanelOpened()) return false; //UI가 열려 있을때 작동 금지.

	AC_Gun* CurWeapon = Cast<AC_Gun>(Weapon);
	CurWeapon->ChangeCurShootingMode();	
	return true;
}

bool AC_GunStrategy::UseRKeyStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	//if (WeaponUser->GetIsHoldDirection()) return false;
	if (WeaponUser->GetHandState() != EHandState::WEAPON_GUN) return false;

	// UI 열려 있을 때 장전 불가 체크
	if (AC_Player* Player = Cast<AC_Player>(WeaponUser))
		if (Player->GetInvenSystem()->GetInvenUI()->GetIsPanelOpened()) return false; //UI가 열려 있을때 작동 금지.	
	
	AC_Gun*	CurWeapon = Cast<AC_Gun>(Weapon);
	if (!IsValid(CurWeapon)) return false;
	
	AC_Item_Bullet* CurBullet = Cast<AC_Item_Bullet>(WeaponUser->GetInvenComponent()->FindMyItemByName(CurWeapon->GetCurrentBulletTypeName()));
	if (!IsValid(CurBullet)) return false;
	
	if (CurBullet->GetItemCurStack() == 0) return false; // 현재 장전할 수 있는 총알이 남아있지 않을 때

	// 현재 Magazine에 최대로 총알이 들어가 있을 때
	if (CurWeapon->GetMaxMagazineBulletCount() == CurWeapon->GetCurMagazineBulletCount()) return false;

	// Draw, Sheath, 탄창 재장전, SR의 경우 Bolt Action 재장전 -> 이 모션 중 하나라도 재생 중이라면 재장전 불가능
	if (CurWeapon->GetIsPlayingMontagesOfAny()) return false;
	
	//if (CurWeapon->GetIsPartAttached(EPartsName::GRIP))
	//{
	//	WeaponUser->GetAttachmentMeshComponent()->DetachFromGun(CurWeapon->GetGunMesh(), EPartsName::GRIP, EAttachmentNames::VERTGRIP);
	//	WeaponUser->GetAttachmentMeshComponent()->DetachFromGun(CurWeapon->GetGunMesh(), EPartsName::MUZZLE, EAttachmentNames::COMPENSATOR);
	//	return true;
	//}
	// WeaponUser->GetAttachmentMeshComponent()->AttachToGun(CurWeapon->GetGunMesh(), EPartsName::GRIP, EAttachmentNames::VERTGRIP);
	//WeaponUser->GetAttachmentMeshComponent()->AttachToGun(CurWeapon->GetGunMesh(), EPartsName::MUZZLE, EAttachmentNames::COMPENSATOR);

	if (AC_SR* CurrentSR = Cast<AC_SR>(CurWeapon)) CurrentSR->SetIsCurrentlyReloadingSRMagazine(true);
	
	CurWeapon->ExecuteMagazineReloadMontage(); // 탄창 재장전
	
	return true;
}

bool AC_GunStrategy::UseMlb_StartedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	//UC_Util::Print("Mlb Clicked");
	AC_Player* CurPlayer = Cast<AC_Player>(WeaponUser);
	if (!IsValid(CurPlayer)) return false;
	if (CurPlayer->GetInvenSystem()->GetInvenUI()->GetIsPanelOpened()) return false; //UI가 열려 있을때 작동 금지.
	
	if (WeaponUser->GetIsHoldDirection()) return false;
	if (!WeaponUser->GetCanFireBullet()) return false;

	if (WeaponUser->GetHandState() != EHandState::WEAPON_GUN) return false;

	AC_Gun* CurWeapon = Cast<AC_Gun>(Weapon);
	
	if (CurWeapon->GetIsPlayingMontagesOfAny() || WeaponUser->GetCharacterMovement()->IsFalling()) return false;
	
	MlbPressTimeCount = 0;

	// 탄창에 한 발도 남지 않은 상황
	if (CurWeapon->GetCurMagazineBulletCount() == 0)
	{
		if (CurWeapon->GetGunSoundData()->NullBulletSound)
		{
			if (CurPlayer) UGameplayStatics::PlaySound2D(this, CurWeapon->GetGunSoundData()->NullBulletSound);
			else UGameplayStatics::PlaySoundAtLocation(this, CurWeapon->GetGunSoundData()->NullBulletSound, CurWeapon->GetActorLocation());
		}
		
		if (CurPlayer) CurPlayer->GetHUDWidget()->GetInformWidget()->AddPlayerWarningLog("THERE IS NO AMMUNITION");
		CurWeapon->ExecuteMagazineReloadMontage(); // 탄창 재장전
		return false;
	}

	// 총알 사격 개시
	bool IsBulletFired = CurWeapon->FireBullet();
	
	// 탄창에 한 발 이상 남은 상황이고, ShootingMode가 SingleShot일 경우
	// SR AimDown, AimDownSight 모두 아닐 경우 & 총알이 1발 이상 남았을 경우 여기가 호출되는 중
	if (CurWeapon->GetCurMagazineBulletCount() > 0 && IsBulletFired)
	{
		if(CurWeapon->GetCurrentShootingMode() == EShootingMode::SR_SINGLE_SHOT && !CurPlayer->GetIsAimDown())
			Cast<AC_SR>(CurWeapon)->ExecuteBoltActionReloadMontage();
	}
	
	return IsBulletFired;
}

bool AC_GunStrategy::UseMlb_OnGoingStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	if (WeaponUser->GetHandState() != EHandState::WEAPON_GUN) return false;
	if (WeaponUser->GetIsHoldDirection()) return false;
	if (!WeaponUser->GetCanFireBullet()) return false;
	AC_Player* CurPlayer = Cast<AC_Player>(WeaponUser);
	if (CurPlayer->GetInvenSystem()->GetInvenUI()->GetIsPanelOpened()) return false; //UI가 열려 있을때 작동 금지.

	AC_Gun* CurWeapon = Cast<AC_Gun>(Weapon);
	
	if (CurWeapon->GetIsPlayingMontagesOfAny() ||
		WeaponUser->GetCharacterMovement()->IsFalling()) return false;


	//총알 연발
	//TODO: 총알의 연사율 받아와서 적용 + 총알 발사 모드에 따라 작동못하게 하기
	if(CurWeapon->GetCurrentShootingMode() != EShootingMode::FULL_AUTO) return false;
	
	MlbPressTimeCount += WeaponUser->GetWorld()->GetDeltaSeconds();
	if (MlbPressTimeCount > CurWeapon->GetBulletRPM())
	{
		MlbPressTimeCount -= CurWeapon->GetBulletRPM();
		if (CurWeapon->GetCurMagazineBulletCount() == 0)
		{
			CurWeapon->ExecuteMagazineReloadMontage(); // 탄창 재장전
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
	if (CurPlayer->GetIsAimDown() && CurWeapon->GetCurrentShootingMode() == EShootingMode::SR_SINGLE_SHOT && CurWeapon->GetCurMagazineBulletCount() >= 0)
		Cast<AC_SR>(CurWeapon)->ExecuteBoltActionReloadMontage();
	
	if (CurWeapon->GetIsPlayingMontagesOfAny() || WeaponUser->GetCharacterMovement()->IsFalling()) return false;
	
	return true;
}

bool AC_GunStrategy::UseMrb_StartedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	if (WeaponUser->GetHandState() != EHandState::WEAPON_GUN) return false;
	AC_Player* CurPlayer = Cast<AC_Player>(WeaponUser);
	if (!IsValid(CurPlayer)) return false;
	if (CurPlayer->GetInvenSystem()->GetInvenUI()->GetIsPanelOpened()) return false; //UI가 열려 있을때 작동 금지.
	if (WeaponUser->GetIsHoldDirection()) return false;
	if (WeaponUser->GetIsTooCloseToAimGun()) return false;
	if (WeaponUser->GetIsActivatingConsumableItem()) return false;

	MrbPressTimeCount = 0;
	AC_Gun* CurWeapon = Cast<AC_Gun>(Weapon);
	
	if (CurWeapon->GetIsPlayingMontagesOfAny() || WeaponUser->GetCharacterMovement()->IsFalling())
		return false;
	
	if (WeaponUser->GetNextSpeed() != 0 && WeaponUser->GetPoseState() == EPoseState::CRAWL) return false;
	
	return CurWeapon->SetAimingPress();
}

bool AC_GunStrategy::UseMrb_OnGoingStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)
{
	if (WeaponUser->GetHandState() != EHandState::WEAPON_GUN)			return false;
	AC_Player* CurPlayer = Cast<AC_Player>(WeaponUser);
	if (!IsValid(CurPlayer))											return false;
	if (CurPlayer->GetInvenSystem()->GetInvenUI()->GetIsPanelOpened())	return false; //UI가 열려 있을때 작동 금지.
	if (WeaponUser->GetIsHoldDirection())								return false;
	if (WeaponUser->GetIsTooCloseToAimGun())							return false;
	if (WeaponUser->GetIsActivatingConsumableItem())					return false;

	//if (!WeaponUser->GetCanFireBullet()) return false;

	AC_Gun* CurWeapon = Cast<AC_Gun>(Weapon);
	if (CurWeapon->GetIsPlayingMontagesOfAny() || WeaponUser->GetCharacterMovement()->IsFalling()) return false;


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
	if (WeaponUser->GetIsTooCloseToAimGun()) return false;
	if (WeaponUser->GetIsActivatingConsumableItem()) return false;

	//if (!WeaponUser->GetCanFireBullet()) return false;

	//MrbPressTimeCount = 0;
	if (WeaponUser->GetMesh()->GetAnimInstance()->Montage_IsPlaying(Weapon->GetCurDrawMontage().AnimMontage)) return false;
	if (WeaponUser->GetMesh()->GetAnimInstance()->Montage_IsPlaying(Weapon->GetCurSheathMontage().AnimMontage)) return false;
	AC_Player* CurUser = Cast<AC_Player>(WeaponUser);
	AC_Gun* CurWeapon = Cast<AC_Gun>(Weapon);
	if (CurWeapon->GetIsPlayingMontagesOfAny() || WeaponUser->GetCharacterMovement()->IsFalling()) return false;


	if (!IsValid(CurUser))   return false;
	if (!IsValid(CurWeapon)) return false;

	if (!CurWeapon->GetIsAimPress() && MrbPressTimeCount < 0.2)
	{
		if (WeaponUser->GetNextSpeed() != 0 && WeaponUser->GetPoseState() == EPoseState::CRAWL)
		{
			return false;
		}

		UGameplayStatics::PlaySound2D(CurWeapon, CurWeapon->GetGunSoundData()->AimDownSightSound);
		return CurWeapon->SetAimingDown();
	}
	if (CurWeapon->GetIsAimPress())
	{
		UGameplayStatics::PlaySound2D(CurWeapon, CurWeapon->GetGunSoundData()->AimDownSightSound, 1.f, 1.5f);
		return CurWeapon->BackToMainCamera();
	}
	CurWeapon->BackToMainCamera();
	CurWeapon->SetIsAimPress(false);

	//CurUser->BackToMainCamera();
	return true;
}


