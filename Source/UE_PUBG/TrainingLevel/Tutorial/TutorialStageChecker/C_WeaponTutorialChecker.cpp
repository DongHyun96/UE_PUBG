// Fill out your copyright notice in the Description page of Project Settings.


#include "C_WeaponTutorialChecker.h"

#include "TrainingLevel/Tutorial/C_TutorialManager.h"

UC_WeaponTutorialChecker::UC_WeaponTutorialChecker()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_WeaponTutorialChecker::BeginPlay()
{
	Super::BeginPlay();

	TutorialStage = ETutorialStage::WeaponTutorial;
	
	// 무기와 탄환 획득하기
	GoalData.Add(FGoalData());
	GoalData[0].SubGoalsAchieved.Init(false, 2);

	// R 재장전하기
	GoalData.Add(FGoalData());

	// 사격장으로 이동하기
	GoalData.Add(FGoalData());

	// 스코프 모드에서 목표물 명중하기
	GoalData.Add(FGoalData());
	GoalData[3].SubGoalsAchieved.Init(false, 10);
}

void UC_WeaponTutorialChecker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
