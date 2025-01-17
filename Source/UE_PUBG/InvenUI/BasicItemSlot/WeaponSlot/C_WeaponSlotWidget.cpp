// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/WeaponSlot/C_WeaponSlotWidget.h"
#include "Character/Component/C_EquippedComponent.h"


void UC_WeaponSlotWidget::UpdateWidget()
{
	if (!OwnerPlayer) return;

	EWeaponSlot curWeaponSlot{};

	switch (WeaponType)
	{
	case EWeaponSlot::MAIN_GUN:
		curWeaponSlot = EWeaponSlot::MAIN_GUN;
		break;
	case EWeaponSlot::SUB_GUN:
		curWeaponSlot = EWeaponSlot::SUB_GUN;
		break;
	case EWeaponSlot::MELEE_WEAPON:
		curWeaponSlot = EWeaponSlot::MELEE_WEAPON;
		break;
	case EWeaponSlot::THROWABLE_WEAPON:
		curWeaponSlot = EWeaponSlot::THROWABLE_WEAPON;
		break;
	default:
		return;
	}

	//TODO : Gun의 경우 사용하는 총알의 종류와 갯수를 표시해줌. 하위 클래스에서 진행할 예정.
	if (AC_Weapon* CachedWeapon = OwnerPlayer->GetEquippedComponent()->GetWeapons()[curWeaponSlot])
	{
		WeaponImage->SetBrushFromTexture(CachedWeapon->GetItemDatas().ItemSlotImage);
		WeaponName->SetText(FText::FromString(CachedWeapon->GetItemDatas().ItemName));
		FSlateBrush Brush = WeaponImage->GetBrush();
		Brush.TintColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
		WeaponImage->SetBrush(Brush);
	}
	else
	{
		WeaponImage->SetBrushFromTexture(nullptr);
		FSlateBrush Brush = WeaponImage->GetBrush();
		Brush.TintColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.3f);
		WeaponImage->SetBrush(Brush);
		SetVisibility(ESlateVisibility::Hidden); //TODO : Hidden과 Collaps? 이거 차이 생각하기.
	}
}
