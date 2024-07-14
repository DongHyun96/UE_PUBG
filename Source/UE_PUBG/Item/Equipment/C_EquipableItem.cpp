// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equipment/C_EquipableItem.h"

void AC_EquipableItem::BeginPlay()
{
	Super::BeginPlay();
}

AC_EquipableItem::AC_EquipableItem()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_EquipableItem::Interaction(AC_BasicCharacter* character)
{
}
