// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TutorialManager.h"

#include "C_TutorialStageChecker.h"
#include "Components/BoxComponent.h"
#include "Engine/TriggerBox.h"
#include "Utility/C_Util.h"

AC_TutorialManager::AC_TutorialManager()
{
	PrimaryActorTick.bCanEverTick = true;
	
	for (int i = 0; i < static_cast<uint8>(ETutorialStage::Max); ++i)
	{
		ETutorialStage Stage = static_cast<ETutorialStage>(i);

		FTutorialStageChecker StageInfo{};
		
		FString StageCheckerName		= GetTutorialStageName(Stage) + "Checker";
		StageInfo.TutorialStageChecker 	= CreateDefaultSubobject<UC_TutorialStageChecker>(FName(*StageCheckerName));
		
		TutorialStageInfos.Add(Stage, StageInfo);
	}
}

void AC_TutorialManager::BeginPlay()
{
	Super::BeginPlay();

	// Bind Start Trigger call back functions
	for (int i = 0; i < static_cast<uint8>(ETutorialStage::Max); ++i)
	{
		ETutorialStage Stage = static_cast<ETutorialStage>(i);

		UC_TutorialStageChecker* StageChecker = TutorialStageInfos[Stage].TutorialStageChecker;

		if (!TutorialStageInfos[Stage].StageStartTriggerBox)
		{
			UC_Util::Print("From AC_TutorialManager::BeginPlay : StageStartTriggerBox not inited!", FColor::Red, 10.f);
			continue;
		}
		
		// TutorialStageInfos[Stage].StageStartTriggerBox->OnComponentBeginOverlap.AddDynamic(StageChecker, &UC_TutorialStageChecker::OnStartTriggerBoxBeginOverlap);
		TutorialStageInfos[Stage].StageStartTriggerBox->OnActorBeginOverlap.AddDynamic(StageChecker, &UC_TutorialStageChecker::OnStartTriggerBoxBeginOverlap);
	}
}

void AC_TutorialManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FString AC_TutorialManager::GetTutorialStageName(ETutorialStage Stage)
{
	switch (Stage)
	{
	case ETutorialStage::MovementTutorial:	return "MovementTutorial";
	case ETutorialStage::WeaponTutorial:	return "WeaponTutorial";
	case ETutorialStage::ThrowableTutorial: return "ThrowableTutorial";
	case ETutorialStage::HealingTutorial:	return "HealingTutorial";
	case ETutorialStage::TutorialEnd:		return "TutorialEnd";
	case ETutorialStage::Max:				return "Max";
	}
	return FString();
}
