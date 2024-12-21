// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUserInterface/C_EquipmentPanel.h"

void UC_EquipmentPanel::NativeConstruct()
{
	Super::NativeConstruct();
	if (OwnerCharacter)
		SetWidgetOwnerCharacter(OwnerCharacter);
	InitializeWidget();

}

void UC_EquipmentPanel::InitializeWidget()
{
	MainGunWidget  ->SetWeaponBoxNum(1);
	SubGunWidget   ->SetWeaponBoxNum(2);
				   
	MeleeWidget    ->SetWeaponBoxNum(4);
	ThrowableWidget->SetWeaponBoxNum(5);

	HelmetWidget  ->SetSlotItemType(EItemTypes::HELMET);
	BackPackWidget->SetSlotItemType(EItemTypes::BACKPACK);
	ArmorWidget   ->SetSlotItemType(EItemTypes::VEST);
	UnKnownSlot   ->SetSlotItemType(EItemTypes::NONE);
	
	MainGunWidget  ->Init();
	SubGunWidget   ->Init();
	MeleeWidget    ->Init();
	ThrowableWidget->Init();
	HelmetWidget   ->Init();
	BackPackWidget ->Init();
	ArmorWidget    ->Init();
	UnKnownSlot    ->Init();
}

void UC_EquipmentPanel::SetWidgetOwnerCharacter(AC_BasicCharacter* inOwnerCharacter)
{
	MainGunWidget  ->SetOwnerCharacter(inOwnerCharacter);
	SubGunWidget   ->SetOwnerCharacter(inOwnerCharacter);
	MeleeWidget    ->SetOwnerCharacter(inOwnerCharacter);
	ThrowableWidget->SetOwnerCharacter(inOwnerCharacter);
	HelmetWidget   ->SetOwnerCharacter(inOwnerCharacter);
	BackPackWidget ->SetOwnerCharacter(inOwnerCharacter);
	ArmorWidget    ->SetOwnerCharacter(inOwnerCharacter);
	UnKnownSlot    ->SetOwnerCharacter(inOwnerCharacter);
}
