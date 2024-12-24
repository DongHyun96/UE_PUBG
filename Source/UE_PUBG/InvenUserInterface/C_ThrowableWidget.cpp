// Fill out your copyright notice in the Description page of Project Settings.

#include "InvenUserInterface/C_ThrowableWidget.h"
#include "InvenUserInterface/C_InvenUiWidget.h"

#include "Character/Component/C_EquippedComponent.h"

#include "Item/C_Item.h"
#include "Item/Weapon/C_Weapon.h"

#include "Character/Component/C_EquippedComponent.h"

#include "Utility/C_Util.h"


void UC_ThrowableWidget::NativeConstruct()
{
	switch (WeaponSlotType)
	{
	case EWeaponSlot::MELEE_WEAPON:
		WeaponNum->SetText(FText::FromString("4"));
		break;
	case EWeaponSlot::THROWABLE_WEAPON:
		WeaponNum->SetText(FText::FromString("5"));
		break;
	default:
		break;
	}
}

FReply UC_ThrowableWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 우클릭인지 체크
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		if (CachedItem)
		{   // 우클릭 이벤트 실행
			if (CachedItem->LegacyMoveToInven(OwnerCharacter))
			{
				OwnerCharacter->GetEquippedComponent()->SetSlotWeapon(WeaponSlotType, nullptr);
				CachedItem = nullptr;
			}
			else
			{
				CachedItem->LegacyMoveToAround(OwnerCharacter);
				OwnerCharacter->GetEquippedComponent()->SetSlotWeapon(WeaponSlotType, nullptr);
				CachedItem = nullptr;
			}
			//SetVisibility(ESlateVisibility::Hidden);

			if (UC_InvenUiWidget* InvenUiWidget = GetTypedOuter<UC_InvenUiWidget>())
				InvenUiWidget->InitListView();
			Init();
			return FReply::Handled();
		}
	}
	else
	{
		UC_Util::Print("No cached item to interact with!", FColor::Red, 5.0f);
	}
	// 다른 버튼 클릭 처리
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UC_ThrowableWidget::Init()
{
	if (IsValid(OwnerCharacter))
	{
		CachedItem = OwnerCharacter->GetEquippedComponent()->GetWeapons()[WeaponSlotType];
	}

	if (IsValid(CachedItem))
	{
		ItemIcon->SetBrushFromTexture(CachedItem->GetItemDatas().ItemIcon);
		ItemName->SetText(FText::FromString(CachedItem->GetItemDatas().ItemName));
		SetVisibility(ESlateVisibility::Visible);
		FSlateBrush Brush = ItemIcon->GetBrush();

		Brush.TintColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f); // 완전 불투명
		ItemIcon->SetBrush(Brush);
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}

void UC_ThrowableWidget::SetWeapon(AC_Item* item)
{
	switch (item->GetItemDatas().ItemType)
	{
	case EItemTypes::MELEEWEAPON:
		if (WeaponSlotType == EWeaponSlot::MELEE_WEAPON)
			CachedItem = Cast<AC_Weapon>(item);
		break;
	case EItemTypes::THROWABLE:
		if (WeaponSlotType == EWeaponSlot::THROWABLE_WEAPON)
			CachedItem = Cast<AC_Weapon>(item);
	default:
		break;
	}
}

void UC_ThrowableWidget::SetWeaponBoxNum(uint8 Num)
{
	WeaponNum->SetText(FText::FromString(FString::FromInt(Num)));

	switch (Num)
	{
	case 4:
		WeaponSlotType = EWeaponSlot::MELEE_WEAPON;
		break;
	case 5:
		WeaponSlotType = EWeaponSlot::THROWABLE_WEAPON;
		break;
	default:
		break;
	}

	Init();
}
