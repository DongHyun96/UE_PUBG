// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem/Healing/C_MedKit.h"
#include "Utility/C_Util.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Character/Component/C_StatComponent.h"

#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_ConsumableUsageMeshComponent.h"
#include "Item/Weapon/C_Weapon.h"

#include "HUD/C_HUDWidget.h"

const float AC_MedKit::SWITCHING_TIME = 3.2f;
const float AC_MedKit::BAND_USAGE_MESH_SHOW_TIME = 10.f;

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

	//if (ConsumableItemState == EConsumableItemState::ACTIVATING)
	//{
	//
	//}
}

void AC_MedKit::HandleActivatingState()
{
	if (AC_Player* Player = Cast<AC_Player>(ItemUser)) 
		Player->GetHUDWidget()->OnActivatingHealUp(100.f, UsageTime, UsingTimer);
}

void AC_MedKit::OnStartUsing()
{
	UsingTimer	= 0.f;
	UsageTime	= 8.f;
	BlockUsed	= 0;

	UC_Util::Print("Starts to use MedKit!");

	//ItemUser->GetConsumableUsageMeshComponent()->ToggleMeshUsageVisible(EConsumableUsageMeshType::, true);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AC_MedKit::SwitchingBandageToSyringe, SWITCHING_TIME, false);
}

void AC_MedKit::OnActivatingFinish()
{
	ItemUser->GetConsumableUsageMeshComponent()->ToggleMeshUsageVisible(EConsumableUsageMeshType::SYRINGE, false);
}

void AC_MedKit::OnCancelActivating()
{
	ItemUser->GetConsumableUsageMeshComponent()->ToggleMeshUsageVisible(EConsumableUsageMeshType::BANDAGE, false);
	ItemUser->GetConsumableUsageMeshComponent()->ToggleMeshUsageVisible(EConsumableUsageMeshType::SYRINGE, false);
}

void AC_MedKit::HandleDestroy()
{
	this->Destroy();
}

void AC_MedKit::SwitchingBandageToSyringe()
{
	if (ConsumableItemState != EConsumableItemState::ACTIVATING)
	{
		ItemUser->GetConsumableUsageMeshComponent()->ToggleMeshUsageVisible(EConsumableUsageMeshType::BANDAGE, false);
		ItemUser->GetConsumableUsageMeshComponent()->ToggleMeshUsageVisible(EConsumableUsageMeshType::SYRINGE, false);
		return;
	}

	ItemUser->GetConsumableUsageMeshComponent()->ToggleMeshUsageVisible(EConsumableUsageMeshType::BANDAGE, true);
	ItemUser->GetConsumableUsageMeshComponent()->ToggleMeshUsageVisible(EConsumableUsageMeshType::SYRINGE, true);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AC_MedKit::HideBandageMesh, BAND_USAGE_MESH_SHOW_TIME, false);
}

void AC_MedKit::HideBandageMesh()
{
	ItemUser->GetConsumableUsageMeshComponent()->ToggleMeshUsageVisible(EConsumableUsageMeshType::BANDAGE, false);
}
