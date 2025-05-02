// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUserInterface/C_EquipmentPanel.h"
#include "Character/C_Player.h"

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
	AC_Player* OwnerPlayer = Cast<AC_Player>(inOwnerCharacter);
	MainGunWidget  ->SetOwnerCharacter(OwnerPlayer);
	SubGunWidget   ->SetOwnerCharacter(OwnerPlayer);
	MeleeWidget    ->SetOwnerCharacter(OwnerPlayer);
	ThrowableWidget->SetOwnerCharacter(OwnerPlayer);
	HelmetWidget   ->SetOwnerCharacter(OwnerPlayer);
	BackPackWidget ->SetOwnerCharacter(OwnerPlayer);
	ArmorWidget    ->SetOwnerCharacter(OwnerPlayer);
	UnKnownSlot    ->SetOwnerCharacter(OwnerPlayer);
}


