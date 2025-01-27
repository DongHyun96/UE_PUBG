// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_GunSlotWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"
#include "InvenUI/BasicItemSlot/EquipmentSlot/C_AttachableItemSlotWidget.h"
#include "Character/Component/C_InvenSystem.h"

#include "InvenUserInterface/C_DragDropOperation.h"

#include "Item/C_Item.h"
#include "Item/Weapon/Gun/C_Gun.h"

#include "Utility/C_Util.h"

void UC_GunSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeAttachSlotMap();
}

bool UC_GunSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UC_Util::Print("Dropped Item");
	//Cast<UC_DragDropOperation>(InOperation)
	UC_DragDropOperation* MyOperation = Cast<UC_DragDropOperation>(InOperation);
	AC_Item* DroppedItem = Cast<AC_Item>(MyOperation->Payload);
	//InOperation->
	if (!DroppedItem) return false;

	HandleDrop(MyOperation);
	OwnerPlayer->GetInvenSystem()->GetInvenUI()->UpdateWidget();
	return true;
	//switch (DroppedItem->GetItemDatas().ItemType)
	//{
	//case EItemTypes::NONE:
	//case EItemTypes::BULLET:
	//case EItemTypes::CONSUMPTIONITEM:
	//	//return false;
	//case EItemTypes::BACKPACK:
	//case EItemTypes::MAINGUN:
	//case EItemTypes::MELEEWEAPON:
	//case EItemTypes::THROWABLE:
	//case EItemTypes::ATTACHMENT:
	//case EItemTypes::VEST:
	//case EItemTypes::HELMET:
	//	HandleDrop(MyOperation);
	//	OwnerPlayer->GetInvenSystem()->GetInvenUI()->UpdateWidget();
	//	return true;
	//default:
	//	return false;
	//}
}

bool UC_GunSlotWidget::MouseRBDownInteraction(AC_Weapon* inSlotWeapon)
{
    return inSlotWeapon->MoveToAround(OwnerPlayer);
}

void UC_GunSlotWidget::UpdateWidget()
{
	//UC_EquippedComponent* EquipComp = OwnerPlayer->GetEquippedComponent();
	//
	//AC_Gun* curWeapon = Cast<AC_Gun>(EquipComp->GetWeapons()[WeaponType]);
	//
	//AC_Weapon* CachedWeapon = OwnerPlayer->GetEquippedComponent()->GetWeapons()[WeaponType];
	////TODO : Gun의 경우 사용하는 총알의 종류와 갯수를 표시해줌. 하위 클래스에서 진행할 예정.
	//
	//UpdateSlotItemImage(curWeapon);

	Super::UpdateWidget();

	UpdateAttachableSlotVisibility();

}

void UC_GunSlotWidget::UpdateAttachableSlotVisibility()
{
	UC_EquippedComponent* EquipComp = OwnerPlayer->GetEquippedComponent();

	AC_Gun* curWeapon = Cast<AC_Gun>(EquipComp->GetWeapons()[WeaponType]);

	if (!curWeapon) return;

	TArray<EPartsName> Parts = curWeapon->GetAttachableParts();
	
	for (auto& AttachSlot : AttachSlotWidgets)
	{
		if (UC_AttachableItemSlotWidget* PartSlot = AttachSlot.Value)
		{
			PartSlot->DiscriminateSlot(Parts, curWeapon);
			//PartSlot->UpdateSlotItemImage(curWeapon);
		}
	}

}

bool UC_GunSlotWidget::HandleDrop(UC_DragDropOperation* InOperation)
{
	UC_EquippedComponent* EquipComp = OwnerPlayer->GetEquippedComponent();
	AC_Weapon* curWeapon = EquipComp->GetWeapons()[WeaponType];
	//AC_Item* DroppedItem = Cast<AC_Item>(InOperation->DraggedItem);
	AC_Item* DroppedItem =InOperation->DraggedItem;

	if (!curWeapon) //return DroppedItem->MoveToSlot(OwnerPlayer); //드랍된 슬롯에 아이템이 없다면 바로 장착
	{
		if (InOperation->curWeaponSlot == EWeaponSlot::MAIN_GUN || InOperation->curWeaponSlot == EWeaponSlot::SUB_GUN)
			if (InOperation->curWeaponSlot != WeaponType)
			{
				// 빈 슬롯과 Gun 슬롯 Swap하는 상황
				if (OwnerPlayer->GetHandState() == EHandState::WEAPON_GUN)
					if (EquipComp->SwapSlotsWhileGunHandState()) return true;
				EquipComp->SetSlotWeapon(InOperation->curWeaponSlot, nullptr);
			}
		EquipComp->SetSlotWeapon(WeaponType, Cast<AC_Weapon>(DroppedItem));
		//DroppedItem->MoveToSlot(OwnerPlayer);
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

bool UC_GunSlotWidget::ChangedGunSlot(AC_Gun* gun)
{
	//자식단계에서 override해서 사용.
	return false;
}

void UC_GunSlotWidget::InitializeAttachSlotMap()
{
	AttachSlotWidgets.Empty();

	// 에디터에서 가져온 위젯을 TMap에 추가
	if (WB_MuzzleSlot)
	{
		AttachSlotWidgets.Add(EPartsName::MUZZLE, WB_MuzzleSlot);
	}

	if (WB_MagazineSlot)
	{
		AttachSlotWidgets.Add(EPartsName::MAGAZINE, WB_MagazineSlot);
	}

	if (WB_ScopeSlot)
	{
		AttachSlotWidgets.Add(EPartsName::SCOPE, WB_ScopeSlot);
	}

	if (WB_StockSlot)
	{
		AttachSlotWidgets.Add(EPartsName::GUNSTOCK, WB_StockSlot);
	}

	if (WB_GripSlot)
	{
		AttachSlotWidgets.Add(EPartsName::GRIP, WB_GripSlot);
	}
}

