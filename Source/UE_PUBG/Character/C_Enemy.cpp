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
    // Test용 weapon spawn들
    /*FActorSpawnParameters Param{};
    Param.Owner = OwnerCharacter;
    AC_MeleeWeapon* MeleeTemp = GetWorld()->SpawnActor<AC_MeleeWeapon>(WeaponClasses[EWeaponSlot::MELEE_WEAPON], Param);
    MeleeTemp->SetOwnerCharacter(OwnerCharacter);
    MeleeTemp->AttachToHolster(OwnerCharacter->GetMesh());

    SetSlotWeapon(EWeaponSlot::MELEE_WEAPON, MeleeTemp);

    FActorSpawnParameters Param2{};
    Param2.Owner = OwnerCharacter;
    AC_Gun* ARTemp = GetWorld()->SpawnActor<AC_Gun>(WeaponClasses[EWeaponSlot::MAIN_GUN], Param2);
    ARTemp->SetOwnerCharacter(OwnerCharacter);
    ARTemp->AttachToHolster(OwnerCharacter->GetMesh());

    SetSlotWeapon(EWeaponSlot::MAIN_GUN, ARTemp);*/

    /*FActorSpawnParameters Param3{};
    Param3.Owner = OwnerCharacter;
    AC_ThrowingWeapon* ThrowTemp = GetWorld()->SpawnActor<AC_ThrowingWeapon>(WeaponClasses[EWeaponSlot::THROWABLE_WEAPON], Param3);
    ThrowTemp->SetOwnerCharacter(OwnerCharacter);
    ThrowTemp->AttachToHolster(OwnerCharacter->GetMesh());

    SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, ThrowTemp);*/

    //AC_ThrowingWeapon::InitTestPool(OwnerCharacter, WeaponClasses[EWeaponSlot::THROWABLE_WEAPON], this);

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
    TSubclassOf<AC_ThrowingWeapon> GrenadeClass         = EquippedComponent->GetGrenadeWeaponClass();
    TSubclassOf<AC_ThrowingWeapon> FlashBangClass       = EquippedComponent->GetFlashBangWeaponClass();
    TSubclassOf<AC_ThrowingWeapon> SmokeGrenadeClass    = EquippedComponent->GetSmokeGrenadeWeaponClass();

    AC_ThrowingWeapon* ThrowingWeapon = GetWorld()->SpawnActor<AC_ThrowingWeapon>(GrenadeClass, Param);
    ThrowingWeapon->MoveToSlot(this);

    ThrowingWeapon = GetWorld()->SpawnActor<AC_ThrowingWeapon>(FlashBangClass, Param);
    ThrowingWeapon->MoveToSlot(this);

    ThrowingWeapon = GetWorld()->SpawnActor<AC_ThrowingWeapon>(SmokeGrenadeClass, Param);
    ThrowingWeapon->MoveToSlot(this);

    //for (auto& pair : EquippedComponent->GetThrowableClassMap())
    //{
    //    AC_ThrowingWeapon* ThrowableWeapon = GetWorld()->SpawnActor<AC_ThrowingWeapon>(pair.Value);
    //    ThrowableWeapon->MoveToSlot(this);
    //}

    // TODO : 다른 Item들 (탄, Consumable item 등등 inven에 넣어두기)
}

