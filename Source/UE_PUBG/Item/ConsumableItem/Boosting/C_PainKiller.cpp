// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem/Boosting/C_PainKiller.h"

AC_PainKiller::AC_PainKiller()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_PainKiller::BeginPlay()
{
	Super::BeginPlay();
}

void AC_PainKiller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AC_PainKiller::StartUsingConsumableItem(AC_BasicCharacter* InItemUser)
{
	return false;
}

void AC_PainKiller::HandleActivatingState()
{
}

void AC_PainKiller::HandleActivateCompletedState()
{
}
