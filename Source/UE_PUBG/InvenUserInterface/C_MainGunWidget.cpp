// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUserInterface/C_MainGunWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"

#include "Item/Weapon/C_Weapon.h"

void UC_MainGunWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UC_MainGunWidget::Init()
{
	if (IsValid(OwnerCharacter))
	{
		Weapon = OwnerCharacter->GetEquippedComponent()->GetWeapons()[EWeaponSlot::MAIN_GUN];
	}

	if (IsValid(Weapon))
	{
		GunImage->SetBrushFromTexture(Weapon->GetItemDatas().ItemIcon);
		GunName->SetText(FText::FromString(Weapon->GetItemDatas().ItemName));
		SetVisibility(ESlateVisibility::Visible);
		FSlateBrush Brush = GunImage->GetBrush();
		Brush.TintColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f); // 완전 불투명
		GunImage->SetBrush(Brush);
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}

void UC_MainGunWidget::SetWeapon(AC_Item* item)
{
	Weapon = Cast<AC_Weapon>(item);
}

void UC_MainGunWidget::SetWeaponBoxNum(uint8 Num)
{
	WeaponNum->SetText(FText::FromString(FString::FromInt(Num)));
}
