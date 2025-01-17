// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/EquipmentSlot/C_EquipmentSlotWidget.h"
#include "Character/Component/C_InvenComponent.h"

void UC_EquipmentSlotWidget::NativeConstruct()
{
}

FReply UC_EquipmentSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Unhandled();
}

void UC_EquipmentSlotWidget::UpdateWidget()
{
	if (!OwnerPlayer) return;
	
	EEquipSlot curItemEquipSlot;

	//슬롯이 어떤 슬롯이냐에 따라 확인해야할 슬롯을 배정. 여기서 하나의 함수로 다 처리하는게 나은가? 아니면 자식단계에서 오버라이드 해서 하는게 좋은가?
	switch (SlotItemType)
	{
	case EItemTypes::HELMET:
		curItemEquipSlot = EEquipSlot::HELMET;
		break;
	case EItemTypes::VEST:
		curItemEquipSlot = EEquipSlot::VEST;
		break;
	case EItemTypes::BACKPACK:
		curItemEquipSlot = EEquipSlot::BACKPACK;
		break;
	//case EItemTypes::ATTACHMENT: 이건 AttachableItemSlotWidget에서 오버라이드해서 사용하기. 
	//TODO : AttachableItemSlotWidget에서 구현하기.
	default:
		return;
	}

	if (AC_EquipableItem* CachedItem = OwnerPlayer->GetInvenComponent()->GetEquipmentItems()[curItemEquipSlot])
	{
		ItemImage->SetBrushFromTexture(CachedItem->GetItemDatas().ItemSlotImage);
		FSlateBrush Brush = ItemImage->GetBrush();
		Brush.TintColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f); 
		ItemImage->SetBrush(Brush);
		SetVisibility(ESlateVisibility::Visible); //TODO : 필요한 기능인가? 적절한 기능인가? 체크하고 결정하기.
	}
	else
	{
		ItemImage->SetBrushFromTexture(nullptr);
		FSlateBrush Brush = ItemImage->GetBrush();
		Brush.TintColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.3f); 
		ItemImage->SetBrush(Brush);
	}
	//TODO : Vest와 Helmet은 내구도 표시가 필요함. 2가지의 표시가 있음. 숫자와 붉은bar, 이건 하위클래스에서 하기.
}
