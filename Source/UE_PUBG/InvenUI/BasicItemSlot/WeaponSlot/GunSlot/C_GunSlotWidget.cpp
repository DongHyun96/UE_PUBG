// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_GunSlotWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"
#include "InvenUI/BasicItemSlot/EquipmentSlot/C_AttachableItemSlotWidget.h"
#include "Character/Component/C_InvenSystem.h"

#include "InvenUserInterface/C_DragDropOperation.h"

#include "Item/C_Item.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Attachment/C_AttachableItem.h"

#include "Utility/C_Util.h"

void UC_GunSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeAttachSlotMap();
}


//FReply UC_GunSlotWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
//	{
//		//for (auto& AttachSlot : AttachSlotWidgets)
//		//{
//		//	if (UC_AttachableItemSlotWidget* PartSlot = AttachSlot.Value)
//		//	{
//		//		// �ڽ� ������ �̺�Ʈ ó�� ��� Ȯ��
//		//		FReply ChildReply = PartSlot->NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
//		//
//		//		if (ChildReply.IsEventHandled()) // �ڽ� ������ �̺�Ʈ�� ó���� ���
//		//		{
//		//			return ChildReply; // ó���� ��� ��ȯ
//		//		}
//		//	}
//		//}sd
//		return FReply::Unhandled();
//
//		AC_Weapon* SlotItem = OwnerPlayer->GetEquippedComponent()->GetWeapons()[WeaponType];
//
//		if (SlotItem)
//		{
//			//�巡�� �̺�Ʈ ����.
//			//�巡�׸� �����ϰ� ������
//			FEventReply RePlyResult =
//				UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
//
//			//UC_Util::Print("LeftMouseButton");
//			OwnerPlayer->GetInvenSystem()->GetInvenUI()->SetIsDragging(true);
//
//			return RePlyResult.NativeReply;
//		}
//	}
//	return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
//}

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
	////TODO : Gun�� ��� ����ϴ� �Ѿ��� ������ ������ ǥ������. ���� Ŭ�������� ������ ����.
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

bool UC_GunSlotWidget::SetAttachmentSlotOnDrop(AC_Weapon* InSlotWeapon, AC_AttachableItem* InAttachableItem)
{
	AC_Gun* SlotGun = Cast<AC_Gun>(InSlotWeapon);
	
	if (!SlotGun) return false;

	if (InAttachableItem->GetItemDatas().ItemPlace == EItemPlace::INVEN)
	{
		OwnerPlayer->GetInvenComponent()->RemoveItemToMyList(InAttachableItem);
	}
	else if (InAttachableItem->GetItemDatas().ItemPlace == EItemPlace::AROUND)
	{
		OwnerPlayer->GetInvenComponent()->RemoveItemToAroundList(InAttachableItem);
	}

	AC_AttachableItem* ChangedItem = SlotGun->GetAttachableItem()[InAttachableItem->GetName()];

	if (ChangedItem)
	{
		if (!ChangedItem->MoveToInven(OwnerPlayer))
			ChangedItem->MoveToAround(OwnerPlayer);
	}
	SlotGun->SetAttachableItemSlot(InAttachableItem->GetName(), InAttachableItem);

	return true;
	//if (!ChangedItem) return true;
	//
	//return ChangedItem->MoveToInven(OwnerPlayer);

	
}

void UC_GunSlotWidget::SetOwnerPlayer(AC_Player* InOwnerPlayer)
{
	OwnerPlayer = InOwnerPlayer;

	if (WB_MuzzleSlot)
		WB_MuzzleSlot->SetOwnerPlayer(InOwnerPlayer);

	if (WB_MagazineSlot)
		WB_MagazineSlot->SetOwnerPlayer(InOwnerPlayer);

	if (WB_ScopeSlot)
		WB_ScopeSlot->SetOwnerPlayer(InOwnerPlayer);

	if (WB_StockSlot)
		WB_StockSlot->SetOwnerPlayer(InOwnerPlayer);

	if (WB_GripSlot)
		WB_GripSlot->SetOwnerPlayer(InOwnerPlayer);

}

bool UC_GunSlotWidget::HandleDrop(UC_DragDropOperation* InOperation)
{
	UC_EquippedComponent* EquipComp = OwnerPlayer->GetEquippedComponent();
	AC_Weapon* curWeapon = EquipComp->GetWeapons()[WeaponType];//����� ������ Weapon
	//AC_Item* DroppedItem = Cast<AC_Item>(InOperation->DraggedItem);
	AC_Item* DroppedItem =InOperation->DraggedItem;

	if (!curWeapon) //return DroppedItem->MoveToSlot(OwnerPlayer); //����� ���Կ� ���Ⱑ ���ٸ� �ٷ� ����
	{
		//��ӵ� ���Կ� �������� ���ٸ� ����
		if (InOperation->curWeaponSlot == EWeaponSlot::MAIN_GUN || InOperation->curWeaponSlot == EWeaponSlot::SUB_GUN)
		{
			if (InOperation->curWeaponSlot != WeaponType)
			{
				// �� ���԰� Gun ���� Swap�ϴ� ��Ȳ
				if (OwnerPlayer->GetHandState() == EHandState::WEAPON_GUN)
					if (EquipComp->SwapSlotsWhileGunHandState()) return true;
				EquipComp->SetSlotWeapon(InOperation->curWeaponSlot, nullptr);
			}
			EquipComp->SetSlotWeapon(WeaponType, Cast<AC_Weapon>(DroppedItem));
			//DroppedItem->MoveToSlot(OwnerPlayer);
			return true;
		}
	}
	if (curWeapon == DroppedItem) return false; //�巡�׵� �����۰� ����� ������ �������� ���� �������̶�� return false;
	//��ӵ� ���Կ� �������� �ִٸ� ����
	// 
	//Around�� �����۰� Slot�� �������� ��ü�ϴ� �Ͱ� �ٸ� �������� �������� �̵��ϴ� �� �����ϱ�.

	//Around�� �����۰� Slot�� �������� ��ü�ϴ� �۾��� �����ϰ� MoveToSlot���� ó��
	if (DroppedItem->GetItemDatas().ItemPlace == EItemPlace::AROUND || DroppedItem->GetItemDatas().ItemPlace == EItemPlace::INVEN)
	{
		if (DroppedItem->GetItemDatas().ItemType == EItemTypes::ATTACHMENT)
		{
			

			//��ӵ� �������� �������̶�� ��ӵ� ���Կ� �켱 ����.
			if (SetAttachmentSlotOnDrop(curWeapon, Cast<AC_AttachableItem>(DroppedItem))) return true;
		}
		return DroppedItem->MoveToSlot(OwnerPlayer);
	}

	if (DroppedItem->GetItemDatas().ItemType == EItemTypes::ATTACHMENT)
	{
		if (InOperation->curWeaponSlot != WeaponType)
			Cast<AC_AttachableItem>(DroppedItem)->MoveToSlot(OwnerPlayer);

		//if (InOperation->curWeaponSlot == EWeaponSlot::MAIN_GUN && WeaponType == EWeaponSlot::SUB_GUN)
		//{
		//	Cast<AC_AttachableItem>(DroppedItem)->MoveToSlot(OwnerPlayer);
		//}
		//
		//if (InOperation->curWeaponSlot == EWeaponSlot::SUB_GUN && WeaponType == EWeaponSlot::MAIN_GUN)
		//{
		//	Cast<AC_AttachableItem>(DroppedItem)->MoveToSlot(OwnerPlayer);
		//}
	}

	//if (InOperation->curWeaponSlot == EWeaponSlot::SUB_GUN && WeaponType == EWeaponSlot::MAIN_GUN)


	AC_Gun* Gun = Cast<AC_Gun>(DroppedItem);
	
	if (!Gun) return false;

	return ChangedGunSlot(Gun);
}

bool UC_GunSlotWidget::ChangedGunSlot(AC_Gun* gun)
{
	//�ڽĴܰ迡�� override�ؼ� ���.
	return false;
}

void UC_GunSlotWidget::InitializeAttachSlotMap()
{
	AttachSlotWidgets.Empty();

	// �����Ϳ��� ������ ������ TMap�� �߰�
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

