// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Character/Component/C_EquippedComponent.h"

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

