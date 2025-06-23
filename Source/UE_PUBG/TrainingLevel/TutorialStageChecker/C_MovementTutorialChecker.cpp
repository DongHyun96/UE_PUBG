// Fill out your copyright notice in the Description page of Project Settings.


#include "C_MovementTutorialChecker.h"


UC_MovementTutorialChecker::UC_MovementTutorialChecker()
{
	PrimaryComponentTick.bCanEverTick = true;

	// WASD 이동하기 Goal
	FGoalData BasicMovementGoalData{};
	BasicMovementGoalData.SubGoalsAchieved.Init(false, 4);
	GoalData.Add(BasicMovementGoalData);

	// Shift 전력질주하기 Goal
	GoalData.Add(FGoalData());

	// 지형 지물 넘기
	GoalData.Add(FGoalData());
}


void UC_MovementTutorialChecker::BeginPlay()
{
	Super::BeginPlay();
}


void UC_MovementTutorialChecker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

