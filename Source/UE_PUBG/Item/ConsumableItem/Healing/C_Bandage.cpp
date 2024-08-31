// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem/Healing/C_Bandage.h"

AC_Bandage::AC_Bandage()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_Bandage::BeginPlay()
{
	BeginPlay();
}

void AC_Bandage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AC_Bandage::StartUsingConsumableItem(AC_BasicCharacter* InItemUser)
{
	return false;
}

void AC_Bandage::HandleActivatingState()
{
}

void AC_Bandage::HandleActivateCompletedState()
{
}
