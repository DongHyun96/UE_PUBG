// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/Panel/ItemPanel/EquipmentPanel/C_EquipmentPanelWdiget.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_MainGunSlotWidget.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_SubGunSlotWidget.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/C_MeleeWeaponSlotWidget.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/C_ThrowableWeaponSlotWidget.h"

#include "InvenUI/BasicItemSlot/EquipmentSlot/C_EquipmentSlotWidget.h"
#include "InvenUI/BasicItemSlot/EquipmentSlot/C_HelmetSlotWidget.h"
#include "InvenUI/BasicItemSlot/EquipmentSlot/C_VestSlotWidget.h"
#include "InvenUI/BasicItemSlot/EquipmentSlot/C_BackPackSlotWidget.h"

void UC_EquipmentPanelWdiget::UpdateWidget()
{
	if (MainGunSlot)
		MainGunSlot->UpdateWidget();

	if (SubGunSlot)
		SubGunSlot->UpdateWidget();

	if (MeleeSlot)
		MeleeSlot->UpdateWidget();

	if (ThrowableSlot)
		ThrowableSlot->UpdateWidget();

	if (HelmetSlot)
		HelmetSlot->UpdateWidget();

	if (BackPackSlot)
		BackPackSlot->UpdateWidget();

	if (VestSlot)
		VestSlot->UpdateWidget();

}

void UC_EquipmentPanelWdiget::SetOwnerPlayer(AC_Player* InOwnerPlayer)
{
	OwnerPlayer = InOwnerPlayer;

	MainGunSlot->SetOwnerPlayer(InOwnerPlayer);

	SubGunSlot->SetOwnerPlayer(InOwnerPlayer);

	MeleeSlot->SetOwnerPlayer(InOwnerPlayer);

	ThrowableSlot->SetOwnerPlayer(InOwnerPlayer);

	HelmetSlot->SetOwnerPlayer(InOwnerPlayer);

	BackPackSlot->SetOwnerPlayer(InOwnerPlayer);

	VestSlot->SetOwnerPlayer(InOwnerPlayer);

}
