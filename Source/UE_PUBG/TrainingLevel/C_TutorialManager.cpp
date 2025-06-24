// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TutorialManager.h"

#include "TutorialStageChecker/C_TutorialStageChecker.h"
#include "Engine/TriggerBox.h"
#include "TutorialStageChecker/C_HealingTutorialChecker.h"
#include "TutorialStageChecker/C_MovementTutorialChecker.h"
#include "TutorialStageChecker/C_ThrowableTutorialChecker.h"
#include "TutorialStageChecker/C_WeaponTutorialChecker.h"
#include "Utility/C_Util.h"

AC_TutorialManager::AC_TutorialManager()
{
	PrimaryActorTick.bCanEverTick = true;
	
	TutorialStageCheckers.Add(ETutorialStage::MovementTutorial, CreateDefaultSubobject<UC_MovementTutorialChecker>("MovementTutorialChecker"));
	TutorialStageCheckers.Add(ETutorialStage::WeaponTutorial, CreateDefaultSubobject<UC_WeaponTutorialChecker>("WeaponTutorialChecker"));
	TutorialStageCheckers.Add(ETutorialStage::ThrowableTutorial, CreateDefaultSubobject<UC_ThrowableTutorialChecker>("ThrowableTutorialChecker"));
	TutorialStageCheckers.Add(ETutorialStage::HealingTutorial, CreateDefaultSubobject<UC_HealingTutorialChecker>("HealingTutorialChecker"));
	TutorialStageCheckers.Add(ETutorialStage::TutorialEnd, CreateDefaultSubobject<UC_TutorialStageChecker>("TutorialEndChecker"));
}

void AC_TutorialManager::BeginPlay()
{
	Super::BeginPlay();

	// Bind Start Trigger call back functions
	for (int i = 0; i < static_cast<uint8>(ETutorialStage::Max); ++i)
	{
		ETutorialStage Stage = static_cast<ETutorialStage>(i);

		if (!StageStartTriggerBoxes.Contains(Stage))
		{
			UC_Util::Print("From AC_TutorialManager::BeginPlay : StageStartTriggerBox not inited!", FColor::Red, 10.f);
			continue;
		}

		if (!IsValid(StageStartTriggerBoxes[Stage]))
		{
			UC_Util::Print("From AC_TutorialManager::BeginPlay : StageStartTriggerBox not valid!", FColor::Red, 10.f);
			continue;
		}
		
		StageStartTriggerBoxes[Stage]->OnActorBeginOverlap.AddDynamic(TutorialStageCheckers[Stage], &UC_TutorialStageChecker::OnStartTriggerBoxBeginOverlap);
	}
}

void AC_TutorialManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_TutorialManager::SetStageToNextStage()
{
	// 현재 Stage Delegate 해제
	if (CurrentStage != ETutorialStage::Max)
		TutorialStageCheckers[CurrentStage]->ClearSubscribedDelegates();

	UC_Util::Print("Set Current Stage to next stage", FColor::Red, 10.f);
	
	if (++CurrentStage != ETutorialStage::Max)
	{
		// TODO : 다음 문 열기
		// 현재는 Delegate Init 처리로 두었음 (For Testing)
		TutorialStageCheckers[CurrentStage]->InitDelegateSubscriptions();
	}
	else
	{
		// Tutorial End 처리
	}
}
