// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUserInterface/C_EquipSlot.h"

#include "Item/C_Item.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "Item/Equipment/C_BackPack.h"

#include "Character/Component/C_InvenComponent.h"

#include "Components/Image.h"

void UC_EquipSlot::NativeConstruct()
{
	Init();
}

FReply UC_EquipSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		if (EquippedItem)
		{   // ��Ŭ�� �̺�Ʈ ����
			//EquippedItem->Interaction(OwnerCharacter);
			EquippedItem->MoveToAround(OwnerCharacter);
			EquippedItem = nullptr;
			//InitInvenUIWidget();
		
			//NativeOnListItemObjectSet������ ȣ��� �ߺ����� �ϴ� �ּ�ó��, �ٸ� �̺�Ʈ�ÿ� �ʱ�ȭ�� �ʿ��ϸ� ����ؾ� �� �� ����.
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
	// �ٸ� ��ư Ŭ�� ó��
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UC_EquipSlot::Init()
{
	if (IsValid(OwnerCharacter))
		EquippedItem = Cast<AC_EquipableItem>(OwnerCharacter->GetInvenComponent()->GetMyBackPack());


	if (!IsValid(EquippedItem))
	{
		ItemImage1->SetBrushFromTexture(nullptr);
		FSlateBrush Brush = ItemImage1->GetBrush();
		Brush.TintColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.3f); // ���� ������
		ItemImage1->SetBrush(Brush);
		//SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ItemImage1->SetBrushFromTexture(EquippedItem->GetItemDatas().ItemIcon);
		FSlateBrush Brush = ItemImage1->GetBrush();
		Brush.TintColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f); // ���� ������
		ItemImage1->SetBrush(Brush);
		SetVisibility(ESlateVisibility::Visible);
	}
}
