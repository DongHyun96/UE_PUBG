// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUserInterface/C_EquipSlot.h"

#include "Item/C_Item.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "Item/Equipment/C_BackPack.h"

#include "Character/Component/C_InvenComponent.h"
#include "Utility/C_Util.h"
#include "Components/Image.h"

void UC_EquipSlot::NativeConstruct()
{
	Init();
}

FReply UC_EquipSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		if (CachedItem)
		{   // 우클릭 이벤트 실행
			//EquippedItem->Interaction(OwnerCharacter);
			//CachedItem->LegacyMoveToAround(OwnerCharacter);
			CachedItem = nullptr;
			//InitInvenUIWidget();
		
			//NativeOnListItemObjectSet에서의 호출과 중복으로 일단 주석처리, 다만 이벤트시에 초기화가 필요하면 사용해야 할 수 있음.
			//if (!CachedItem) return;
		
			//InitBar(CachedItem);
			//SetVisibility(ESlateVisibility::Visible);
			Init();
			//EquippedItem->SetOwnerCharacter(nullptr);
			if (UC_InvenUiWidget* InvenUiWidget = GetTypedOuter<UC_InvenUiWidget>())
				InvenUiWidget->InitWidget();
			return FReply::Handled();
		}
	}
	else
	{
		//UC_Util::Print("No cached item to interact with!", FColor::Red, 5.0f);
	}
	// 다른 버튼 클릭 처리
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UC_EquipSlot::Init()
{
	if (IsValid(OwnerCharacter))
		DivisionInit();
	//	CachedItem = Cast<AC_EquipableItem>(OwnerCharacter->GetInvenComponent()->GetMyBackPack());
	 
	if (!IsValid(CachedItem))
	{
		ItemImage1->SetBrushFromTexture(nullptr);
		FSlateBrush Brush = ItemImage1->GetBrush();
		Brush.TintColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.3f); // 완전 불투명
		ItemImage1->SetBrush(Brush);
		//SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		if (CachedItem->GetItemDatas()->ItemType != SlotItemType) return;

		ItemImage1->SetBrushFromTexture(CachedItem->GetItemDatas()->ItemSlotImage);
		FSlateBrush Brush = ItemImage1->GetBrush();
		Brush.TintColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f); // 완전 불투명
		ItemImage1->SetBrush(Brush);
		SetVisibility(ESlateVisibility::Visible);
	}
}

void UC_EquipSlot::DivisionInit()
{
	switch (SlotItemType)
	{
	case EItemTypes::HELMET:
		CachedItem = Cast<AC_EquipableItem>(OwnerCharacter->GetInvenComponent()->GetEquipmentItems()[EEquipSlot::HELMET]);
		break;
	case EItemTypes::VEST:
		CachedItem = Cast<AC_EquipableItem>(OwnerCharacter->GetInvenComponent()->GetEquipmentItems()[EEquipSlot::VEST]);
		break;
	case EItemTypes::BACKPACK:
		CachedItem = Cast<AC_EquipableItem>(OwnerCharacter->GetInvenComponent()->GetEquipmentItems()[EEquipSlot::BACKPACK]);
		break;
	default:
		break;
	}
}


