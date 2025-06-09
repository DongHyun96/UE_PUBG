// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/WeaponSlot/C_MeleeWeaponSlotWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/C_Util.h"

bool UC_MeleeWeaponSlotWidget::MouseRBDownInteraction(AC_Weapon* inSlotWeapon)
{

	if (OwnerPlayer->GetIsActivatingConsumableItem()) return false;

	if (inSlotWeapon->MoveToInven(OwnerPlayer, inSlotWeapon->GetItemCurStack()))
	{
		UC_Util::Print("RightButtonDown");
		UGameplayStatics::PlaySound2D(inSlotWeapon, inSlotWeapon->GetPickUpSound());
		return true;
	}
	
	return false;
}


