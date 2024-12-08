
// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem/Healing/C_FirstAidKit.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Character/Component/C_StatComponent.h"

#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_ConsumableUsageMeshComponent.h"
#include "Item/Weapon/C_Weapon.h"

#include "HUD/C_HUDWidget.h"

#include "Utility/C_Util.h"

AC_FirstAidKit::AC_FirstAidKit()
{
	PrimaryActorTick.bCanEverTick = true;

	UsageTime = 6.f;

	//0.24ÃÊ¾¿ -> +15 : ÃÑ 5¹ø ÇØ¼­ ÃÑ 2ÃÊ¿¡ ÃÖ´ë 75¸¸Å­ Èú
	OneBlockHealAmount	= 15.f;
	OneBlockTime		= 0.24f;
	TotalUsingBlockCnt	= 5;
	HealUpLimit			= 75.f;
	
}

void AC_FirstAidKit::BeginPlay()
{
	Super::BeginPlay();
}

void AC_FirstAidKit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



void AC_FirstAidKit::HandleActivatingState()
{
	if (AC_Player* Player = Cast<AC_Player>(ItemUser)) 
		Player->GetHUDWidget()->OnActivatingHealUp(75.f, UsageTime, UsingTimer);
}

void AC_FirstAidKit::OnStartUsing()
{
	UsingTimer	= 0.f;
	UsageTime	= 6.f;
	BlockUsed	= 0;
	UC_Util::Print("Starts to use FirstAidKit!");
	ItemUser->GetConsumableUsageMeshComponent()->ToggleMeshUsageVisible(EConsumableUsageMeshType::SYRINGE, true);
}

void AC_FirstAidKit::OnActivatingFinish()
{
	ItemUser->GetConsumableUsageMeshComponent()->ToggleMeshUsageVisible(EConsumableUsageMeshType::SYRINGE, false);
}

void AC_FirstAidKit::OnCancelActivating()
{
	ItemUser->GetConsumableUsageMeshComponent()->ToggleMeshUsageVisible(EConsumableUsageMeshType::SYRINGE, false);
}
