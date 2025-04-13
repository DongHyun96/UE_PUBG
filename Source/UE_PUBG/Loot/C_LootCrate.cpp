// Fill out your copyright notice in the Description page of Project Settings.


#include "Loot/C_LootCrate.h"
#include "Character/C_BasicCharacter.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "Singleton/C_GameSceneManager.h"
#include "Item/ItemManager/C_ItemManager.h"
#include "Utility/C_Util.h"

// Sets default values
AC_LootCrate::AC_LootCrate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	//SetActorTickEnabled(false);
}

// Called when the game starts or when spawned
void AC_LootCrate::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AC_LootCrate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AC_LootCrate::SetCharacterLootCrate(AC_BasicCharacter* InCharacter)
{
	if (!InCharacter) return false;

	UC_InvenComponent*	  InvenComp = InCharacter->GetInvenComponent();
	UC_EquippedComponent* EquipComp = InCharacter->GetEquippedComponent();

    AddInventoryItemsToLoot(InvenComp);

    AddEquipmentItemsToLoot(InvenComp);

    AddEquippedWeaponsToLoot(EquipComp);

    //오름차순 정렬
    LootItems.Sort([](const AC_Item& A, const AC_Item& B)
        {
            return A.GetItemType() < B.GetItemType();
        });

	SetActorEnableCollision(false);
	SetActorEnableCollision(true); 

	return true;
}

void AC_LootCrate::AddInventoryItemsToLoot(UC_InvenComponent* InvenComp)
{
    const TMap<FName, TArray<AC_Item*>>& InvenItemsMap = InvenComp->GetMyItems();

    for (const TPair<FName, TArray<AC_Item*>>& Pair : InvenItemsMap)
    {
        const TArray<AC_Item*>& Items = Pair.Value;
        for (AC_Item* Item : Items)
        {
            if (Item)
            {
                Item->SetOwnerCharacter(nullptr);
                Item->SetItemPlace(EItemPlace::AROUND);
                Item->SetOwner(this);

                LootItems.Add(Item);
            }
        }
    }
}

void AC_LootCrate::AddEquipmentItemsToLoot(UC_InvenComponent* InvenComp)
{
    TMap<EEquipSlot, AC_EquipableItem*> SlotItemsMap = InvenComp->GetEquipmentItems();

    for (const TPair<EEquipSlot, AC_EquipableItem*>& Pair : SlotItemsMap)
    {
        AC_Item* Item = Cast<AC_Item>(Pair.Value);
        if (Item)
        {
            InvenComp->SetSlotEquipment(Pair.Key, nullptr);
            Item->SetOwnerCharacter(nullptr);
            Item->SetItemPlace(EItemPlace::AROUND);
            Item->SetOwner(this);
            Item->SetActorEnableCollision(false);
            LootItems.Add(Item);
        }
    }
}

void AC_LootCrate::AddEquippedWeaponsToLoot(UC_EquippedComponent* EquipComp)
{
    TMap<EWeaponSlot, AC_Weapon*> EquippedWeaponsMap = EquipComp->GetWeapons();

    // TODO : 총에 있는 총알을 빼서 스폰해주기.
    AC_Gun* MainGun = Cast<AC_Gun>(EquippedWeaponsMap[EWeaponSlot::MAIN_GUN]);

    AC_Gun* SubGun = Cast<AC_Gun>(EquippedWeaponsMap[EWeaponSlot::SUB_GUN]);

    int curAmmoCount = 0;

    if (MainGun)
    {
        curAmmoCount = MainGun->GetCurBulletCount();
        if (curAmmoCount != 0) //장전된 총알이 없다면 총알을 스폰하지 않음.
        {
            AC_Item* LeftBullet = GAMESCENE_MANAGER->GetItemManager()->SpawnItem(MainGun->GetCurrentBulletTypeName(), FVector(0, 0, 0), curAmmoCount);
			if (!LeftBullet)
			{
				UC_Util::Print("LeftBullet is nullptr");
				return;
			}
            LeftBullet->SetOwner(this);
            LeftBullet->SetActorEnableCollision(false);
            this->AddItemInLootItems(LeftBullet); 
            MainGun->SetCurBulletCount(0);
        }
    }

    if (SubGun)
    {
        curAmmoCount = SubGun->GetCurBulletCount();
        if (curAmmoCount != 0) //장전된 총알이 없다면 총알을 스폰하지 않음.
        {
            AC_Item* LeftBullet = GAMESCENE_MANAGER->GetItemManager()->SpawnItem(SubGun->GetCurrentBulletTypeName(), FVector(0, 0, 0), curAmmoCount);
            LeftBullet->SetOwner(this);
            LeftBullet->SetActorEnableCollision(false);
            this->AddItemInLootItems(LeftBullet);
            SubGun->SetCurBulletCount(0);
        }
    }

    for (const TPair<EWeaponSlot, AC_Weapon*>& Pair : EquippedWeaponsMap)
    {
        AC_Item* Weapon = Cast<AC_Item>(Pair.Value);
        if (Weapon) // 무기가 존재하는 경우
        {
            //Weapon->SetOwnerCharacter(nullptr);
            EquipComp->SetSlotWeapon(Pair.Key, nullptr);
            Weapon->SetItemPlace(EItemPlace::AROUND);
            Weapon->SetOwner(this);
            Weapon->SetActorEnableCollision(false);
            //
            LootItems.Add(Weapon);
        }
    }
}

