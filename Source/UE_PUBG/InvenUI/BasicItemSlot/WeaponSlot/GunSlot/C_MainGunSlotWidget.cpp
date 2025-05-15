// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_MainGunSlotWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"
#include "Item/Weapon/Gun/C_Gun.h"

bool UC_MainGunSlotWidget::ChangedGunSlot(AC_Gun* gun)
{
	if (!gun) return false;
	UC_EquippedComponent* EquipComp = OwnerPlayer->GetEquippedComponent();

	if (OwnerPlayer->GetHandState() == EHandState::WEAPON_GUN)
		if (EquipComp->SwapSlotsWhileGunHandState()) return true;

	EquipComp->SetSlotWeapon(EWeaponSlot::SUB_GUN, nullptr);//우선 드래그된 아이템 슬롯의 장착을 해제.

	AC_Weapon* curSlotGun = EquipComp->SetSlotWeapon(WeaponType, gun);//드랍된 슬롯에 드래그된 아이템을 장착

	if (IsValid(EquipComp->SetSlotWeapon(EWeaponSlot::SUB_GUN, curSlotGun)))//드랍된 슬롯에 장착되어 있었던 아이템을 드래그된 아이템 슬롯에 장착.
		return true;
	else
		return false;
}


