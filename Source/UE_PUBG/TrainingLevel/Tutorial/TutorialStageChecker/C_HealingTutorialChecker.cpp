// Fill out your copyright notice in the Description page of Project Settings.


#include "C_HealingTutorialChecker.h"

#include "Character/C_Player.h"
#include "Item/ConsumableItem/C_BoostItem.h"
#include "Item/ConsumableItem/C_ConsumableItem.h"
#include "Singleton/C_GameSceneManager.h"
#include "TrainingLevel/Tutorial/C_TutorialManager.h"
#include "TrainingLevel/Tutorial/TutorialWidget/C_TutorialGoalExplanation.h"
#include "TrainingLevel/Tutorial/TutorialWidget/C_TutorialGoalExplanationContainer.h"
#include "TrainingLevel/Tutorial/TutorialWidget/C_TutorialWidget.h"

UC_HealingTutorialChecker::UC_HealingTutorialChecker()
{
	PrimaryComponentTick.bCanEverTick = true;
	TutorialStage = ETutorialStage::HealingTutorial;
}

void UC_HealingTutorialChecker::BeginPlay()
{
	Super::BeginPlay();
	
	// 회복 아이템 획득하기
	GoalData.Add(FGoalData());

	// 구급상자 사용하기
	GoalData.Add(FGoalData());

	// 부스트 아이템 획득하기
	GoalData.Add(FGoalData());

	// 부스트 게이지 채우기
	GoalData.Add(FGoalData());
}


void UC_HealingTutorialChecker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UC_HealingTutorialChecker::InitStage()
{
	Super::InitStage();

	// HealingTutorial 모든 Goal 해당 Delegate로 확인 구독
	AC_ConsumableItem::HealingTutorialDelegate.BindUObject(this, &UC_HealingTutorialChecker::OnGoalAchieved);

	// Goal Explanation 내에서의 CurBoostAmount 내용 업데이트 구독
	UC_TutorialGoalExplanationContainer* HealTutorialGoalExplanationContainer =
		OwnerTutorialManager->GetTutorialWidget()->GetTutorialGoalExplanationContainer(ETutorialStage::HealingTutorial);

	// Boosting Gauge 정보 업데이트 처리
	UC_TutorialGoalExplanation* BoostingGoalExplanation = HealTutorialGoalExplanationContainer->GetTutorialGoalExplanation(3);

	UC_StatComponent* PlayerStatComponent = GAMESCENE_MANAGER->GetPlayer()->GetStatComponent(); 
	PlayerStatComponent->HealingTutorialBoostAmountNumberDelegate.
	AddUObject(BoostingGoalExplanation, &UC_TutorialGoalExplanation::UpdateNumberInfoOnGoalWidget);

	AC_BoostItem::HealingTutorialUpdateNumberDelegate.AddUObject(BoostingGoalExplanation, &UC_TutorialGoalExplanation::UpdateNumberInfoOnGoalWidget);
	BoostingGoalExplanation->UpdateNumberInfoOnGoalWidget(PlayerStatComponent->GetCurBoosting());

	SubscribedDelegates.Add(AC_ConsumableItem::HealingTutorialDelegate);
	SubscribedWidgetNumberUpdaterDelegates.Add(PlayerStatComponent->HealingTutorialBoostAmountNumberDelegate);
	SubscribedWidgetNumberUpdaterDelegates.Add(AC_BoostItem::HealingTutorialUpdateNumberDelegate);

	// HealingTutorialStage의 경우, 예외적으로 Player의 HP를 5로 시작
	GAMESCENE_MANAGER->GetPlayer()->GetStatComponent()->SetCurHP(5.f);
}
