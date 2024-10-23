// Fill out your copyright notice in the Description page of Project Settings.

#include "InvenUserInterface/C_ThrowableWidget.h"
#include "Item/C_Item.h"
#include "Item/Weapon/C_Weapon.h"

void UC_ThrowableWidget::SetWeapon(AC_Item* item)
{
	Weapon = Cast<AC_Weapon>(item);
}

void UC_ThrowableWidget::SetWeaponBoxNum(uint8 Num)
{
	WeaponNum->SetText(FText::FromString(FString::FromInt(Num)));
}
