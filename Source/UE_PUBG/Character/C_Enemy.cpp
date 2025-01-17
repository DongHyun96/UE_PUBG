// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_SwimmingComponent.h"
#include "Character/Component/C_PoseColliderHandlerComponent.h"

#include "Item/C_Item.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Weapon/Gun/C_AR.h"
#include "Item/Weapon/MeleeWeapon/C_MeleeWeapon.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"


AC_Enemy::AC_Enemy()
{
	TeamID = ETeamAttitude::Hostile;
}

void AC_Enemy::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = 600.f;

    // TODO : 비행기 타기 이전에 spawn하는 것으로 수정하기
    SpawnDefaultWeaponsAndItemsForSelf();
	
}

bool AC_Enemy::SetPoseState(EPoseState InChangeFrom, EPoseState InChangeTo)
{
	if (!bCanMove)											return false;
	if (bIsJumping || GetCharacterMovement()->IsFalling())	return false;
	if (InChangeFrom == InChangeTo)							return false;
	if (SwimmingComponent->IsSwimming())					return false;

	switch (InChangeTo)
	{
	case EPoseState::STAND:
		switch (InChangeFrom)
		{
		case EPoseState::CROUCH: // Crouch To Stand (Pose transition 없이 바로 처리)

			if (!PoseColliderHandlerComponent->CanChangePoseOnCurrentSurroundEnvironment(EPoseState::STAND)) return false;

			Super::SetPoseState(EPoseState::STAND);
			return true;

		case EPoseState::CRAWL: // Crawl To Stand

			if (bIsActivatingConsumableItem) return false;
			if (!PoseColliderHandlerComponent->CanChangePoseOnCurrentSurroundEnvironment(EPoseState::STAND)) return false;

			if (SwimmingComponent->GetSwimmingState() != ESwimmingState::ON_GROUND)
			{
				Super::SetPoseState(EPoseState::STAND);
				return true;
			}

			ExecutePoseTransitionAction(GetPoseTransitionMontagesByHandState(HandState).CrawlToStand, EPoseState::STAND);
			return true;

		case EPoseState::POSE_MAX: default: return false;
		}
	case EPoseState::CROUCH:
		switch (InChangeFrom)
		{
		case EPoseState::STAND: // Stand To Crouch

			if (!PoseColliderHandlerComponent->CanChangePoseOnCurrentSurroundEnvironment(EPoseState::CROUCH)) return false;
			Super::SetPoseState(EPoseState::CROUCH);
			return true;

		case EPoseState::CRAWL: // Crawl To Crouch

			if (bIsActivatingConsumableItem) return false; // TODO : 일어설 수 없습니다 UI 띄우기
			if (!PoseColliderHandlerComponent->CanChangePoseOnCurrentSurroundEnvironment(EPoseState::CROUCH)) return false;
			
			ExecutePoseTransitionAction(GetPoseTransitionMontagesByHandState(HandState).CrawlToCrouch, EPoseState::CROUCH);

			return true;

		case EPoseState::POSE_MAX: default: return false;
		}
	case EPoseState::CRAWL:
		switch (InChangeFrom)
		{
		case EPoseState::STAND: // Stand to Crawl

			if (bIsActivatingConsumableItem) return false; // TODO : 없드릴 수 없습니다 UI 띄우기
			if (!PoseColliderHandlerComponent->CanChangePoseOnCurrentSurroundEnvironment(EPoseState::CRAWL)) return false;

			ExecutePoseTransitionAction(GetPoseTransitionMontagesByHandState(HandState).StandToCrawl, EPoseState::CRAWL);

			return true;

		case EPoseState::CROUCH: // Crouch to Crawl

			if (bIsActivatingConsumableItem) return false; // TODO : 없드릴 수 없습니다 UI 띄우기
			if (!PoseColliderHandlerComponent->CanChangePoseOnCurrentSurroundEnvironment(EPoseState::CRAWL)) return false;

			ExecutePoseTransitionAction(GetPoseTransitionMontagesByHandState(HandState).CrouchToCrawl, EPoseState::CRAWL);
			return true;

		case EPoseState::POSE_MAX: default: return false;
		}
	case EPoseState::POSE_MAX: default: return false;
	}

}

void AC_Enemy::SpawnDefaultWeaponsAndItemsForSelf()
{
    FActorSpawnParameters Param{};
    Param.Owner = this;

    TSubclassOf<AC_Weapon> MeleeWeaponClass = EquippedComponent->GetSubclassOfWeapon(EWeaponSlot::MELEE_WEAPON);
    AC_MeleeWeapon* MeleeWeapon = GetWorld()->SpawnActor<AC_MeleeWeapon>(MeleeWeaponClass, Param);
    EquippedComponent->SetSlotWeapon(EWeaponSlot::MELEE_WEAPON, MeleeWeapon);
    //MeleeWeapon->LegacyMoveToSlot(this);

    TSubclassOf<AC_Weapon> MainGunWeaponClass = EquippedComponent->GetSubclassOfWeapon(EWeaponSlot::MAIN_GUN);
    AC_Gun* MainGun = GetWorld()->SpawnActor<AC_Gun>(MainGunWeaponClass, Param);
    MainGun->MoveToSlot(this);
    //EquippedComponent->SetSlotWeapon(EWeaponSlot::MAIN_GUN, MainGun);

    TSubclassOf<AC_Weapon> SubGunWeaponClass = EquippedComponent->GetSubclassOfWeapon(EWeaponSlot::SUB_GUN);
    AC_Gun* SubGun = GetWorld()->SpawnActor<AC_Gun>(SubGunWeaponClass, Param);
    SubGun->MoveToSlot(this);
    //EquippedComponent->SetSlotWeapon(EWeaponSlot::SUB_GUN, SubGun);

    // Throwable Weapon setting 하기
    for (auto& pair : EquippedComponent->GetSubclassOfThrowingWeapon())
    {
        AC_ThrowingWeapon* ThrowWeapon = GetWorld()->SpawnActor<AC_ThrowingWeapon>(pair.Value, Param);
        ThrowWeapon->MoveToSlot(this);
    }

    // TODO : 다른 Item들 (탄, Consumable item 등등 inven에 넣어두기)
}

