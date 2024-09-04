// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem/Healing/C_MedKit.h"
#include "Utility/C_Util.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Character/Component/C_StatComponent.h"

#include "Character/Component/C_EquippedComponent.h"
#include "Item/Weapon/C_Weapon.h"

AC_MedKit::AC_MedKit()
{
	PrimaryActorTick.bCanEverTick = true;

	UsageTime = 8.f;

	OneBlockHealAmount	= 100.f;
	OneBlockTime		= 0.3f;
	TotalUsingBlockCnt	= 1;
	HealUpLimit			= 100.f;
}

void AC_MedKit::BeginPlay()
{
	Super::BeginPlay();
}

void AC_MedKit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_MedKit::HandleActivatingState()
{
	if (AC_Player* Player = Cast<AC_Player>(ItemUser)) 
		Player->OnActivatingHealUp(100.f, UsageTime, UsingTimer);
}

void AC_MedKit::InitStartVariables()
{
	UsingTimer	= 0.f;
	UsageTime	= 8.f;
	BlockUsed	= 0;

	UC_Util::Print("Starts to use MedKit!");
}
