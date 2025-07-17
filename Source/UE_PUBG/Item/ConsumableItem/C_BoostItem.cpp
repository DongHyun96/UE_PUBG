// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem/C_BoostItem.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "HUD/C_HUDWidget.h"

#include "Utility/C_Util.h"

FTutorialStageUpdateWidgetNumberDelegate AC_BoostItem::HealingTutorialUpdateNumberDelegate{};

AC_BoostItem::AC_BoostItem()
{
	PrimaryActorTick.bCanEverTick = true;
	HealingTutorialLootingGoalIndex = 2;
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

	// HealingTutorial중이고, 아이템 사용 주체가 Player일 때, HealingTutorial Delegate 호출 처리
	if (HealingTutorialDelegate.IsBound() && Cast<AC_Player>(ItemUser))
	{
		float ItemUserCurBoostingAmount = ItemUser->GetStatComponent()->GetCurBoosting(); 

		// 90%를 넘지 않았어도 현재 Boost량이 얼마인지 업데이트 하는 것은 해주어야 함
		if (HealingTutorialUpdateNumberDelegate.IsBound()) HealingTutorialUpdateNumberDelegate.Broadcast(ItemUserCurBoostingAmount);

		if (ItemUserCurBoostingAmount >= 90.f)
			HealingTutorialDelegate.Execute(3, -1); // Boost량 90% 이상일 경우, Goal Achieved
	}
	
	if (ItemCurStack > 1) UsingTimer = 0.f;
	ConsumableItemState = EConsumableItemState::USED;
}

void AC_BoostItem::HandleDestroy()
{
	UC_Util::Print("Destroying Boost Item", FColor::Red, 5.f);
	this->DestroyItem();
}


