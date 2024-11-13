// Fill out your copyright notice in the Description page of Project Settings.

#include "InvenUserInterface/C_ThrowableWidget.h"
#include "InvenUserInterface/C_InvenUiWidget.h"
#include "Item/C_Item.h"
#include "Item/Weapon/C_Weapon.h"

#include "Character/Component/C_EquippedComponent.h"

#include "Utility/C_Util.h"


FReply UC_ThrowableWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// ��Ŭ������ üũ
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		if (Weapon)
		{   // ��Ŭ�� �̺�Ʈ ����
			Weapon->MoveToInven(OwnerCharacter);
			
			Weapon = nullptr;

			//SetVisibility(ESlateVisibility::Hidden);

			if (UC_InvenUiWidget* InvenUiWidget = GetTypedOuter<UC_InvenUiWidget>())
				InvenUiWidget->InitListView();
			return FReply::Handled();
		}
	}
	else
	{
		UC_Util::Print("No cached item to interact with!", FColor::Red, 5.0f);
	}
	// �ٸ� ��ư Ŭ�� ó��
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UC_ThrowableWidget::Init()
{
	if (IsValid(OwnerCharacter))
	{
		Weapon = OwnerCharacter->GetEquippedComponent()->GetWeapons()[WeaponSlotType];
	}

	if (IsValid(Weapon))
	{
		ItemIcon->SetBrushFromTexture(Weapon->GetItemDatas().ItemIcon);
		ItemName->SetText(FText::FromString(Weapon->GetItemDatas().ItemName));
		SetVisibility(ESlateVisibility::Visible);
		FSlateBrush Brush = ItemIcon->GetBrush();

		Brush.TintColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f); // ���� ������
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
			Weapon = Cast<AC_Weapon>(item);
		break;
	case EItemTypes::THROWABLE:
		if (WeaponSlotType == EWeaponSlot::THROWABLE_WEAPON)
			Weapon = Cast<AC_Weapon>(item);
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
