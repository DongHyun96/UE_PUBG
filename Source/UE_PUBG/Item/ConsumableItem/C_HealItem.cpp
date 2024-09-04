// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem/C_HealItem.h"

#include "Character/C_Player.h"

AC_HealItem::AC_HealItem()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_HealItem::BeginPlay()
{
	Super::BeginPlay();
}

void AC_HealItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_HealItem::HandleActivateCompletedState()
{
	float NewHP = FMath::Clamp
	(
		ItemUser->GetStatComponent()->GetCurHP() + OneBlockHealAmount,
		0.f, HealUpLimit
	);

	if (UsingTimer < OneBlockTime)
	{
		if (AC_Player* Player = Cast<AC_Player>(ItemUser))
			Player->OnActivateHealUpCompletedTick(NewHP, UsingTimer / OneBlockTime);
		return;
	}

	// UsingTimer Block 시간 다 채웠을 때

	ItemUser->GetStatComponent()->SetCurHP(NewHP);
	BlockUsed++;
	UsingTimer -= OneBlockTime;

	if (BlockUsed >= TotalUsingBlockCnt ||
		ItemUser->GetStatComponent()->GetCurHP() >= HealUpLimit) // End of HandleActivateCompletedState
	{
		ConsumableItemState = EConsumableItemState::USED;
		return;
	}
}

bool AC_HealItem::IsAvailableToStartUsing(AC_BasicCharacter* InItemUser)
{
	return InItemUser->GetStatComponent()->GetCurHP() < HealUpLimit;
}
