
// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem/Healing/C_FirstAidKit.h"
#include "Character/C_BasicCharacter.h"
#include "Utility/C_Util.h"

AC_FirstAidKit::AC_FirstAidKit()
{
	PrimaryActorTick.bCanEverTick = true;

	UsageTime = 6.f;
}

void AC_FirstAidKit::BeginPlay()
{
	Super::BeginPlay();
}

void AC_FirstAidKit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AC_FirstAidKit::StartUsingConsumableItem(AC_BasicCharacter* InItemUser)
{
	// 최대로 채울 수 있는 체력보다 더 많을 때
	if (InItemUser->GetStatComponent()->GetCurHP() >= InItemUser->GetStatComponent()->GetHealUpLimit()) return false;
	if (ConsumableItemState != EConsumableItemState::IDLE) return false;

	UC_Util::Print("Starts to use first Aid kit");

	// 사용 시작하기
	ItemUser			= InItemUser;
	ConsumableItemState = EConsumableItemState::ACTIVATING;
	UsingTimer			= 0.f;
	UsageTime			= 6.f;
	BlockUsed			= 0;

	if (!UsingMontageMap.Contains(ItemUser->GetPoseState()))    return true;
	if (!UsingMontageMap[ItemUser->GetPoseState()].AnimMontage) return true;

	ItemUser->PlayAnimMontage(UsingMontageMap[ItemUser->GetPoseState()]);

	return true;
}

void AC_FirstAidKit::HandleActivatingState()
{
}

void AC_FirstAidKit::HandleActivateCompletedState()
{
	//const float ONE_BLOCK_HEAL_AMOUNT = 15.f;
	//const float ONE_BLOCK_TIME = 0.24f;
	//const UINT	TOTAL_USING_BLOCK_CNT = 5;
	
	if (UsingTimer < ONE_BLOCK_TIME)
	{
		// TODO : HP HUD 다음 목표치까지 서서히 채우는 것 보여주기
		return;
	}
	
	// UsingTimer Block 시간 다 채웠을 때

	//ItemUser->GetStatComponent()->GetCurHP()
	float NewHP = FMath::Clamp
	(
		ItemUser->GetStatComponent()->GetCurHP() + ONE_BLOCK_HEAL_AMOUNT,
		0.f, ItemUser->GetStatComponent()->GetHealUpLimit()
	);

	ItemUser->SetCurHP(NewHP);
	BlockUsed++;
	UsingTimer -= ONE_BLOCK_TIME;

	if (BlockUsed >= TOTAL_USING_BLOCK_CNT ||
		ItemUser->GetStatComponent()->GetCurHP() >= ItemUser->GetStatComponent()->GetHealUpLimit()) // End of HandleUseConsumableItem
	{
		ConsumableItemState = EConsumableItemState::USED;
		return;
	}


}
