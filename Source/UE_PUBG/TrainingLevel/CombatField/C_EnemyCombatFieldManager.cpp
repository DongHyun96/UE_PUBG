// Fill out your copyright notice in the Description page of Project Settings.


#include "C_EnemyCombatFieldManager.h"

#include "C_CombatFieldManager.h"
#include "Character/C_Player.h"
#include "Components/BoxComponent.h"
#include "Utility/C_Util.h"


UC_EnemyCombatFieldManager::UC_EnemyCombatFieldManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_EnemyCombatFieldManager::BeginPlay()
{
	Super::BeginPlay();
}


void UC_EnemyCombatFieldManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
