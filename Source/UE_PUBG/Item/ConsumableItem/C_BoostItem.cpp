// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem/C_BoostItem.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "HUD/C_HUDWidget.h"

#include "Utility/C_Util.h"


AC_BoostItem::AC_BoostItem()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_BoostItem::BeginPlay()
{
	Super::BeginPlay();
}

void AC_BoostItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AC_BoostItem::IsAvailableToStartUsing(AC_BasicCharacter* InItemUser)
{
	return InItemUser->GetStatComponent()->GetCurBoosting() < 100.f;
}

void AC_BoostItem::HandleActivatingState()
{
	if (AC_Player* Player = Cast<AC_Player>(ItemUser))
		Player->GetHUDWidget()->OnActivatingBooster(UsageTime, UsingTimer);

}

void AC_BoostItem::HandleActivateCompletedState()
{
	ItemUser->GetStatComponent()->AddBoost(BoostAmount);
	if (ItemCurStack > 1)
		UsingTimer = 0.f;
	ConsumableItemState = EConsumableItemState::USED;
}

void AC_BoostItem::HandleDestroy()
{
	UC_Util::Print("Destroying Boost Item", FColor::Red, 5.f);
	this->DestroyItem();
}


