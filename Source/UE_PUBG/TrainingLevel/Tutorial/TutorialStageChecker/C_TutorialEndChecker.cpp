// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TutorialEndChecker.h"

#include "Item/Weapon/Gun/C_TutorialGoalWidget.h"
#include "Singleton/C_GameSceneManager.h"
#include "TrainingLevel/C_TrainingGroundManager.h"
#include "TrainingLevel/Tutorial/C_TutorialStageTriggerBox.h"
#include "Utility/C_Util.h"

UC_TutorialEndChecker::UC_TutorialEndChecker()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_TutorialEndChecker::BeginPlay()
{
	Super::BeginPlay();
	bShowStageExplanationVideo = false;
}


void UC_TutorialEndChecker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UC_TutorialEndChecker::InitStage()
{
	// Super::InitStage();
	UC_Util::Print("Tutorial End Stage InitStage called, Moving to Training Ground", FColor::Red, 10.f);
	
	// Training Ground Default 위치로 Player 배치 
	GAMESCENE_MANAGER->GetTrainingGroundManager()->SetPlayerToDefaultTrainingGroundTransform();
}
