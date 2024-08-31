// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem/Boosting/C_EnergyDrink.h"

AC_EnergyDrink::AC_EnergyDrink()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_EnergyDrink::BeginPlay()
{
	Super::BeginPlay();
}

void AC_EnergyDrink::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AC_EnergyDrink::StartUsingConsumableItem(AC_BasicCharacter* InItemUser)
{
	return false;
}

void AC_EnergyDrink::HandleActivatingState()
{
}

void AC_EnergyDrink::HandleActivateCompletedState()
{
}
