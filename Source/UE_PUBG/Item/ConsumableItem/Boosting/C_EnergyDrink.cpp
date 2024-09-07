// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem/Boosting/C_EnergyDrink.h"
#include "Utility/C_Util.h"

#include "Character/C_Player.h"
#include "Character/C_BasicCharacter.h"

#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_ConsumableUsageMeshComponent.h"
#include "Item/Weapon/C_Weapon.h"


AC_EnergyDrink::AC_EnergyDrink()
{
	PrimaryActorTick.bCanEverTick = true;

	UsageTime = 4.f;
	BoostAmount = 40.f;
}

void AC_EnergyDrink::BeginPlay()
{
	Super::BeginPlay();
}

void AC_EnergyDrink::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_EnergyDrink::OnStartUsing()
{
	UsingTimer = 0.f;
	UsageTime  = 4.f;
	BoostAmount = 40.f;
	UC_Util::Print("Starts to use EnergyDrink!");

	ItemUser->GetConsumableUsageMeshComponent()->ToggleMeshUsageVisible(EConsumableUsageMeshType::DRINK, true);
}

void AC_EnergyDrink::OnActivatingFinish()
{
	ItemUser->GetConsumableUsageMeshComponent()->ToggleMeshUsageVisible(EConsumableUsageMeshType::DRINK, false);
}

void AC_EnergyDrink::OnCancelActivating()
{
	ItemUser->GetConsumableUsageMeshComponent()->ToggleMeshUsageVisible(EConsumableUsageMeshType::DRINK, false);
}
