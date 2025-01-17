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

	//������ � �����̳Ŀ� ���� Ȯ���ؾ��� ������ ����. ���⼭ �ϳ��� �Լ��� �� ó���ϴ°� ������? �ƴϸ� �ڽĴܰ迡�� �������̵� �ؼ� �ϴ°� ������?
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
	//case EItemTypes::ATTACHMENT: �̰� AttachableItemSlotWidget���� �������̵��ؼ� ����ϱ�. 
	//TODO : AttachableItemSlotWidget���� �����ϱ�.
	default:
		return;
	}

	if (AC_EquipableItem* CachedItem = OwnerPlayer->GetInvenComponent()->GetEquipmentItems()[curItemEquipSlot])
	{
		ItemImage->SetBrushFromTexture(CachedItem->GetItemDatas().ItemSlotImage);
		FSlateBrush Brush = ItemImage->GetBrush();
		Brush.TintColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f); 
		ItemImage->SetBrush(Brush);
		SetVisibility(ESlateVisibility::Visible); //TODO : �ʿ��� ����ΰ�? ������ ����ΰ�? üũ�ϰ� �����ϱ�.
	}
	else
	{
		ItemImage->SetBrushFromTexture(nullptr);
		FSlateBrush Brush = ItemImage->GetBrush();
		Brush.TintColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.3f); 
		ItemImage->SetBrush(Brush);
	}
	//TODO : Vest�� Helmet�� ������ ǥ�ð� �ʿ���. 2������ ǥ�ð� ����. ���ڿ� ����bar, �̰� ����Ŭ�������� �ϱ�.
}
