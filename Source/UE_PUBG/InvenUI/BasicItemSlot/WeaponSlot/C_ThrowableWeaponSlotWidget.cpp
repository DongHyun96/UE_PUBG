// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/WeaponSlot/C_ThrowableWeaponSlotWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"

bool UC_ThrowableWeaponSlotWidget::MouseRBDownInteraction(AC_Weapon* inSlotWeapon)
{
	return inSlotWeapon->MoveToInven(OwnerPlayer, inSlotWeapon->GetItemCurStack());
}


