// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem/Boosting/C_PainKiller.h"
#include "Utility/C_Util.h"

#include "Character/C_Player.h"
#include "Character/C_BasicCharacter.h"

#include "Character/Component/C_EquippedComponent.h"

#include "Item/Weapon/C_Weapon.h"

AC_PainKiller::AC_PainKiller()
{
	PrimaryActorTick.bCanEverTick = true;

	UsageTime = 6.f;
	BoostAmount = 60.f;
}

void AC_PainKiller::BeginPlay()
{
	Super::BeginPlay();
}

void AC_PainKiller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_PainKiller::InitStartVariables()
{
	UsingTimer = 0.f;
	UsageTime  = 6.f;
	BoostAmount = 60.f;
	UC_Util::Print("Starts to use PainKiller!");
}
