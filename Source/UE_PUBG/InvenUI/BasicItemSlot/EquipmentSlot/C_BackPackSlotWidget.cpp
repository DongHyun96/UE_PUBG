// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/EquipmentSlot/C_BackPackSlotWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"
#include "Item/Equipment/C_EquipableItem.h"

//
//FReply UC_BackPackSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
//	{
//		if (AC_EquipableItem* SlotItem = OwnerPlayer->GetInvenComponent()->GetEquipmentItems()[EEquipSlot::BACKPACK])
//		{   // ��Ŭ�� �̺�Ʈ ����
//			//EquippedItem->Interaction(OwnerCharacter);
//			SlotItem->MoveToAround(OwnerPlayer);
//			//InitInvenUIWidget();
//
//			//NativeOnListItemObjectSet������ ȣ��� �ߺ����� �ϴ� �ּ�ó��, �ٸ� �̺�Ʈ�ÿ� �ʱ�ȭ�� �ʿ��ϸ� ����ؾ� �� �� ����.
//			//if (!CachedItem) return;
//
//			//InitBar(CachedItem);
//			//SetVisibility(ESlateVisibility::Visible);
//			if (UC_InventoryUIWidget* InvenUiWidget = GetTypedOuter<UC_InventoryUIWidget>())
//				InvenUiWidget->UpdateWidget();
//			return FReply::Handled();
//		}
//	}
//	else
//	{
//		//UC_Util::Print("No cached item to interact with!", FColor::Red, 5.0f);
//	}
//	// �ٸ� ��ư Ŭ�� ó��
//	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
//}
//
//void UC_BackPackSlotWidget::UpdateWidget()
//{
//	AC_EquipableItem* SlotItem = OwnerPlayer->GetInvenComponent()->GetEquipmentItems()[EEquipSlot::BACKPACK];
//
//	UpdateSlotItemImage(SlotItem);
//}
