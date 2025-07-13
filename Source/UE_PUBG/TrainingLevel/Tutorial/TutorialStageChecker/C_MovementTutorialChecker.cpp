// Fill out your copyright notice in the Description page of Project Settings.


#include "C_MovementTutorialChecker.h"

#include "Character/C_Player.h"
#include "Character/Component/C_InputComponent.h"
#include "Singleton/C_GameSceneManager.h"
#include "TrainingLevel/Tutorial/C_TutorialManager.h"
#include "Utility/C_Util.h"


UC_MovementTutorialChecker::UC_MovementTutorialChecker()
{
	PrimaryComponentTick.bCanEverTick = true;
	TutorialStage = ETutorialStage::MovementTutorial;
}


void UC_MovementTutorialChecker::BeginPlay()
{
	Super::BeginPlay();

	// WASD 이동하기 Goal
	GoalData.Add(FGoalData());
	GoalData[0].SubGoalsAchieved.Init(false, 4);

	// Shift 전력질주하기 Goal
	GoalData.Add(FGoalData());

	// 지형 지물 넘기
	GoalData.Add(FGoalData());
}


void UC_MovementTutorialChecker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UC_MovementTutorialChecker::InitStage()
{
	Super::InitStage();

	UC_InputComponent* PlayerInputComponent = GAMESCENE_MANAGER->GetPlayer()->GetInputComponent();

	PlayerInputComponent->MovementTutorialDelegate.BindUObject(this, &UC_MovementTutorialChecker::OnGoalAchieved);
	SubscribedDelegates.Add(PlayerInputComponent->MovementTutorialDelegate);
}

