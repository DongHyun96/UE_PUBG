// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem/Healing/C_Bandage.h"
#include "Utility/C_Util.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"

#include "Character/Component/C_StatComponent.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_ConsumableUsageMeshComponent.h"
#include "Item/Weapon/C_Weapon.h"

#include "HUD/C_HUDWidget.h"

AC_Bandage::AC_Bandage()
{
	PrimaryActorTick.bCanEverTick = true;

	UsageTime = 4.f;

	OneBlockHealAmount	= 2.5f;
	OneBlockTime		= 0.375f;
	TotalUsingBlockCnt	= 4;
	HealUpLimit			= 75.f;
}

void AC_Bandage::BeginPlay()
{
	Super::BeginPlay();
}

void AC_Bandage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_Bandage::HandleActivatingState()
{
	if (AC_Player* Player = Cast<AC_Player>(ItemUser))
	{
		float HealUpDestHPValue = FMath::Clamp(Player->GetStatComponent()->GetCurHP() + 10.f, 0.f, 75.f);
		Player->GetHUDWidget()->OnActivatingHealUp(HealUpDestHPValue, UsageTime, UsingTimer);
	}
}

void AC_Bandage::OnStartUsing()
{
	UsingTimer	= 0.f;
	UsageTime	= 4.f;
	BlockUsed	= 0;

	UC_Util::Print("Starts to use Bandage!");
}

void AC_Bandage::OnActivatingFinish()
{
	ItemUser->GetConsumableUsageMeshComponent()->ToggleMeshUsageVisible(EConsumableUsageMeshType::BANDAGE, true);
	
	// 10초 뒤 팔에 감은 붕대 감추기
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AC_Bandage::HideUsageMesh, USAGE_MESH_SHOWN_TIME, false);
}

void AC_Bandage::OnCancelActivating()
{
	// 아무 action도 안취해도 됨
}

void AC_Bandage::HandleDestroy()
{
	bDestroyFlag = true;
}

void AC_Bandage::HideUsageMesh()
{
	ItemUser->GetConsumableUsageMeshComponent()->ToggleMeshUsageVisible(EConsumableUsageMeshType::BANDAGE, false);
	if (bDestroyFlag)
	{
		UC_Util::Print("Destroying Bandage", FColor::Red, 5.f);
		this->Destroy();
	}
}
