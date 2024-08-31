// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem/C_ConsumableItem.h"

AC_ConsumableItem::AC_ConsumableItem()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_ConsumableItem::BeginPlay()
{
	Super::BeginPlay();
}

void AC_ConsumableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ConsumableItemState == EConsumableItemState::USED) return;

	UsingTimer += DeltaTime;

	switch (ConsumableItemState)
	{
	case EConsumableItemState::IDLE:
		UsingTimer = 0.f;
		return;
	case EConsumableItemState::ACTIVATING:

		if (UsingTimer < UsageTime)
		{
			HandleActivatingState(); // TODO : Update HUD UI Timer
			return;
		}

		ConsumableItemState = EConsumableItemState::ACTIVATE_COMPLETED;
		UsingTimer = 0.f;
		return;
	case EConsumableItemState::ACTIVATE_COMPLETED:
		HandleActivateCompletedState(); // Template Method
		return;
	case EConsumableItemState::USED:
		// TODO : 아이템 삭제
		return;
	default:
		return;
	}
}
