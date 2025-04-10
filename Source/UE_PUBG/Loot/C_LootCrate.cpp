// Fill out your copyright notice in the Description page of Project Settings.


#include "Loot/C_LootCrate.h"
#include "Character/C_BasicCharacter.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Item/Weapon/C_Weapon.h"
#include "Item/Equipment/C_EquipableItem.h"
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
            return A.GetItemName() < B.GetItemName();
        });

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
            Item->SetOwnerCharacter(nullptr);
            Item->SetItemPlace(EItemPlace::AROUND);
            Item->SetOwner(this);

            LootItems.Add(Item);
        }
    }
}

void AC_LootCrate::AddEquippedWeaponsToLoot(UC_EquippedComponent* EquipComp)
{
    TMap<EWeaponSlot, AC_Weapon*> EquipItemsMap = EquipComp->GetWeapons();

    for (const TPair<EWeaponSlot, AC_Weapon*>& Pair : EquipItemsMap)
    {
        AC_Item* Weapon = Cast<AC_Item>(Pair.Value);
        if (Weapon) // 무기가 존재하는 경우
        {
            Weapon->SetOwnerCharacter(nullptr);
            Weapon->SetItemPlace(EItemPlace::AROUND);
            Weapon->SetOwner(this);

            LootItems.Add(Weapon);
        }
    }
}

