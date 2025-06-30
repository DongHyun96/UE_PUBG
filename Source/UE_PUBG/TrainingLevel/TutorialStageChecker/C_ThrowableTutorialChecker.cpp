// Fill out your copyright notice in the Description page of Project Settings.


#include "C_ThrowableTutorialChecker.h"

#include "TrainingLevel/C_TutorialManager.h"


UC_ThrowableTutorialChecker::UC_ThrowableTutorialChecker()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_ThrowableTutorialChecker::BeginPlay()
{
	Super::BeginPlay();

	TutorialStage = ETutorialStage::ThrowableTutorial;
	
	// 투척류 획득하기
	GoalData.Add(FGoalData());

	// 수류탄 연습장으로 이동하기
	GoalData.Add(FGoalData());

	// 수류탄 던지기
	GoalData.Add(FGoalData());
}


void UC_ThrowableTutorialChecker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

