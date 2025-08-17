// Fill out your copyright notice in the Description page of Project Settings.


#include "C_PlayerCombatFieldManager.h"


UC_PlayerCombatFieldManager::UC_PlayerCombatFieldManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_PlayerCombatFieldManager::BeginPlay()
{
	Super::BeginPlay();
}

void UC_PlayerCombatFieldManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

