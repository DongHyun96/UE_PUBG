// Fill out your copyright notice in the Description page of Project Settings.

#include "InvenUserInterface/C_ThrowableWidget.h"
#include "Item/C_Item.h"
#include "Item/Weapon/C_Weapon.h"

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
