// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_GunSlotWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"
#include "Character/Component/C_InvenSystem.h"

#include "InvenUserInterface/C_DragDropOperation.h"

#include "Item/C_Item.h"
#include "Item/Weapon/Gun/C_Gun.h"

#include "Utility/C_Util.h"

bool UC_GunSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UC_Util::Print("Dropped Item");
	//Cast<UC_DragDropOperation>(InOperation)
	UC_DragDropOperation* MyOperation = Cast<UC_DragDropOperation>(InOperation);
	AC_Item* DroppedItem = Cast<AC_Item>(MyOperation->Payload);

	if (!DroppedItem) return false;

	switch (DroppedItem->GetItemDatas().ItemType)
	{
	case EItemTypes::NONE:
	case EItemTypes::BULLET:
	case EItemTypes::CONSUMPTIONITEM:
		//return false;
	case EItemTypes::BACKPACK:
	case EItemTypes::MAINGUN:
	case EItemTypes::MELEEWEAPON:
	case EItemTypes::THROWABLE:
	case EItemTypes::ATTACHMENT:
	case EItemTypes::VEST:
	case EItemTypes::HELMET:
		HandleDrop(DroppedItem);
		OwnerPlayer->GetInvenSystem()->GetInvenUI()->UpdateWidget();
		return true;
	default:
		return false;
	}
}

bool UC_GunSlotWidget::MouseRBDownInteraction(AC_Weapon* inSlotWeapon)
{
    return inSlotWeapon->MoveToAround(OwnerPlayer);
}

bool UC_GunSlotWidget::HandleDrop(AC_Item* DroppedItem)
{
	UC_EquippedComponent* EquipComp = OwnerPlayer->GetEquippedComponent();
	AC_Weapon* curWeapon = EquipComp->GetWeapons()[WeaponType];

	if (!curWeapon) //return DroppedItem->MoveToSlot(OwnerPlayer); //드랍된 슬롯에 아이템이 없다면 바로 장착
	{
		EquipComp->SetSlotWeapon(WeaponType, Cast<AC_Weapon>(DroppedItem));
		return true;
	}
	if (curWeapon == DroppedItem) return false; //드래그된 아이템과 드랍된 슬롯의 아이템이 같은 아이템이라면 return false;
	
	//Around의 아이템과 Slot의 아이템을 교체하는 것과 다른 슬롯으로 아이템을 이동하는 것 구현하기.

	//Around의 아이템과 Slot의 아이템을 교체하는 작업은 간단하게 MoveToSlot으로 처리
	if (DroppedItem->GetItemDatas().ItemPlace == EItemPlace::AROUND)
		return DroppedItem->MoveToSlot(OwnerPlayer);
	AC_Gun* Gun = Cast<AC_Gun>(DroppedItem);
	
	if (!Gun) return false;

	return ChangedGunSlot(Gun);
}

bool UC_GunSlotWidget::ChangedGunSlot(AC_Weapon* gun)
{
	return false;
}

