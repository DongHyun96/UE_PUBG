// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/WeaponSlot/C_ThrowableWeaponSlotWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"
#include "Kismet/GameplayStatics.h"

bool UC_ThrowableWeaponSlotWidget::MouseRBDownInteraction(AC_Weapon* inSlotWeapon)
{

	if (OwnerPlayer->GetIsActivatingConsumableItem()) return false;
	
	if(inSlotWeapon->MoveToInven(OwnerPlayer, inSlotWeapon->GetItemCurStack()))
	{
		UGameplayStatics::PlaySound2D(inSlotWeapon, inSlotWeapon->GetPickUpSound());
		return true;
	}
	return false;
}


