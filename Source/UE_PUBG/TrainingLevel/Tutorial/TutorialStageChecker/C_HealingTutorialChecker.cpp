// Fill out your copyright notice in the Description page of Project Settings.


#include "C_HealingTutorialChecker.h"

#include "TrainingLevel/Tutorial/C_TutorialManager.h"

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

