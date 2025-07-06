// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TutorialManager.h"

#include "C_TutorialStageTriggerBox.h"
#include "Character/C_Player.h"
#include "Components/ShapeComponent.h"
#include "Door/C_TutorialGate.h"
#include "TutorialStageChecker/C_TutorialStageChecker.h"
#include "Engine/TriggerBox.h"
#include "Singleton/C_GameSceneManager.h"
#include "TutorialStageChecker/C_HealingTutorialChecker.h"
#include "TutorialStageChecker/C_MovementTutorialChecker.h"
#include "TutorialStageChecker/C_ThrowableTutorialChecker.h"
#include "TutorialStageChecker/C_WeaponTutorialChecker.h"
#include "TutorialWidget/C_TutorialWidget.h"
#include "Utility/C_Util.h"

AC_TutorialManager::AC_TutorialManager()
{
	PrimaryActorTick.bCanEverTick = true;
	
	TutorialStageCheckers.Add(ETutorialStage::MovementTutorial, CreateDefaultSubobject<UC_MovementTutorialChecker>("MovementTutorialChecker"));
	TutorialStageCheckers.Add(ETutorialStage::WeaponTutorial, CreateDefaultSubobject<UC_WeaponTutorialChecker>("WeaponTutorialChecker"));
	TutorialStageCheckers.Add(ETutorialStage::ThrowableTutorial, CreateDefaultSubobject<UC_ThrowableTutorialChecker>("ThrowableTutorialChecker"));
	TutorialStageCheckers.Add(ETutorialStage::HealingTutorial, CreateDefaultSubobject<UC_HealingTutorialChecker>("HealingTutorialChecker"));
	TutorialStageCheckers.Add(ETutorialStage::TutorialEnd, CreateDefaultSubobject<UC_TutorialStageChecker>("TutorialEndChecker"));

	for (TTuple<ETutorialStage, UC_TutorialStageChecker*> TutorialStageChecker : TutorialStageCheckers)
		TutorialStageChecker.Value->SetOwnerTutorialManager(this);
}

void AC_TutorialManager::BeginPlay()
{
	Super::BeginPlay();

	// Bind Start Trigger call back functions
	// 첫 MovementTutorial Stage는 시간 차를 두고 automatic하게 시작함
	for (int i = 1; i < static_cast<uint8>(ETutorialStage::Max); ++i)
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

	if (TutorialWidget)
	{
		TutorialWidget->AddToViewport(20);
		TutorialWidget->SetTutorialManager(this);
		StartTutorial();
	}
}

void AC_TutorialManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_TutorialManager::StartTutorial()
{
	// Set Player Location
	GAMESCENE_MANAGER->GetPlayer()->SetActorTransform(PlayerTutorialStartTransform);

	// 2초 뒤에 MovementTutorial 시작
	FTimerHandle& TimerHandle = GAMESCENE_MANAGER->GetTimerHandle();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		TutorialWidget->SetStageExplanationPanel(CurrentStage);
		TutorialWidget->ToggleStageExplanationPanel(true);
	}, 2.f, false);
}

void AC_TutorialManager::SetStageToNextStage()
{
	// 현재 Stage Delegate 해제
	if (CurrentStage != ETutorialStage::Max)
	{
		TutorialStageCheckers[CurrentStage]->ClearSubscribedDelegates();
		
		if (TutorialGates.Contains(CurrentStage))
		{
			// TutorialGates[CurrentStage]->ToggleOpeningBoxTriggerEnabled(true);
			TutorialGates[CurrentStage]->OpenGate();
		}
	}

	// 현재 Stage의 Start TriggerBox 비활성화
	if (StageStartTriggerBoxes.Contains(CurrentStage) && IsValid(StageStartTriggerBoxes[CurrentStage]))
		StageStartTriggerBoxes[CurrentStage]->ToggleTriggerBox(false);

	++CurrentStage;

	// 다음 Stage의 Start TriggerBox가 있다면, Start Trigger Box 활성화
	if (StageStartTriggerBoxes.Contains(CurrentStage) && IsValid(StageStartTriggerBoxes[CurrentStage]))
		StageStartTriggerBoxes[CurrentStage]->ToggleTriggerBox(true);
}

void AC_TutorialManager::InitCurrentStageChecker()
{
	TutorialStageCheckers[CurrentStage]->InitStage();
}
