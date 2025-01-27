// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/EquipmentSlot/C_AttachableItemSlotWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"
#include "Item/Attachment/C_AttachableItem.h"
#include "Character/C_Player.h"
#include "Components/Image.h"
#include "Item/Weapon/Gun/C_Gun.h"

void UC_AttachableItemSlotWidget::UpdateSlotItemImage(AC_Gun* SlotItem)
{
	AC_AttachableItem* curItem = SlotItem->GetAttachableItem()[SlotName];

	if (curItem)
	{
		ItemImage->SetBrushFromTexture(curItem->GetItemDatas().ItemSlotImage);
		FSlateBrush Brush = ItemImage->GetBrush();
		Brush.TintColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
		ItemImage->SetBrush(Brush);
		//SetVisibility(ESlateVisibility::Visible); //TODO : �ʿ��� ����ΰ�? ������ ����ΰ�? üũ�ϰ� �����ϱ�.
	}
	else
	{
		ItemImage->SetBrushFromTexture(nullptr);
		FSlateBrush Brush = ItemImage->GetBrush();
		Brush.TintColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.3f);
		ItemImage->SetBrush(Brush);
	}
}

void UC_AttachableItemSlotWidget::DiscriminateSlot(TArray<EPartsName> AttachableSlots, AC_Gun* SlotItem)
{
	for (EPartsName Name : AttachableSlots)
	{
		if (SlotName == Name)
		{
			this->UpdateSlotItemImage(SlotItem);
			this->SetVisibility(ESlateVisibility::Visible); //Widget Ȱ��ȭ.
			return;
		}
	}
	this->SetVisibility(ESlateVisibility::Collapsed); // Widget ��Ȱ��ȭ.
}

//void UC_AttachableItemSlotWidget::DiscriminateSlot(TArray<EPartsName> AttachableSlots, AC_Gun SlotItem)
//{
//	for (EPartsName Name : AttachableSlots)
//	{
//		if (SlotName == Name)
//		{
//			this->UpdateSlotItemImage(SlotItem);
//			this->SetVisibility(ESlateVisibility::Visible); //Widget Ȱ��ȭ.
//			return;
//		}
//	}
//	this->SetVisibility(ESlateVisibility::Collapsed); // Widget ��Ȱ��ȭ.
//}
