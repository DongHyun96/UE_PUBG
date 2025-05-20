// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/WeaponSlot/C_MeleeWeaponSlotWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"

bool UC_MeleeWeaponSlotWidget::MouseRBDownInteraction(AC_Weapon* inSlotWeapon)
{

	if (OwnerPlayer->GetIsActivatingConsumableItem()) return false;

	return inSlotWeapon->MoveToInven(OwnerPlayer, inSlotWeapon->GetItemCurStack());
}


