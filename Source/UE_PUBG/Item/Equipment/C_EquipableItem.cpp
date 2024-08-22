// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equipment/C_EquipableItem.h"

AC_EquipableItem::AC_EquipableItem()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_EquipableItem::BeginPlay()
{
	Super::BeginPlay();
}


void AC_EquipableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_EquipableItem::Interaction(AC_BasicCharacter* character)
{
}
