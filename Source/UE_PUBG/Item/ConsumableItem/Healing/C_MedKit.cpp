// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem/Healing/C_MedKit.h"

AC_MedKit::AC_MedKit()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_MedKit::BeginPlay()
{
	Super::BeginPlay();
}

void AC_MedKit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AC_MedKit::StartUsingConsumableItem(AC_BasicCharacter* InItemUser)
{
	return false;
}

void AC_MedKit::HandleActivatingState()
{
}

void AC_MedKit::HandleActivateCompletedState()
{
}
