// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TutorialStageChecker.h"

#include "Character/C_Player.h"
#include "Item/Weapon/Gun/C_TutorialGoalWidget.h"
#include "Singleton/C_GameSceneManager.h"
#include "TrainingLevel/Tutorial/C_TutorialManager.h"
#include "TrainingLevel/Tutorial/C_TutorialStageTriggerBox.h"
#include "TrainingLevel/Tutorial/TutorialWidget/C_TutorialGoalExplanationContainer.h"
#include "TrainingLevel/Tutorial/TutorialWidget/C_TutorialWidget.h"
#include "Utility/C_Util.h"

UC_TutorialStageChecker::UC_TutorialStageChecker()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_TutorialStageChecker::BeginPlay()
{
	Super::BeginPlay();
	
	// Goal Data Init은 각 자식 클래스에서 결정
	GoalData.Empty();

	// Init GoalTriggerBoxes
	GoalTriggerBoxes = OwnerTutorialManager->GetTriggerBoxesReference(TutorialStage);

	FString Str{};
	
	switch (TutorialStage)
	{
	case ETutorialStage::MovementTutorial: Str = "MovementTutorial";
		break;
	case ETutorialStage::WeaponTutorial: Str = "WeaponTutorial";
		break;
	case ETutorialStage::ThrowableTutorial: Str = "ThrowableTutorial";
		break;
	case ETutorialStage::HealingTutorial: Str = "HealingTutorial";
		break;
	case ETutorialStage::TutorialEnd: Str = "TutorialEnd";
		break;
	case ETutorialStage::Max:
		break;
	}
	
	UC_Util::Print(Str, FColor::MakeRandomColor(), 10.f);
}

void UC_TutorialStageChecker::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	ClearSubscribedDelegates();
}

void UC_TutorialStageChecker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UC_TutorialStageChecker::OnGoalAchieved(uint8 TargetGoal, int SubGoalIndex)
{
	// 실질적인 Main Goal sequence를 따르게끔 처리
	if (TargetGoal != CurrentMainGoalIndex) return false;
	
	if (TargetGoal >= GoalData.Num()) return false;
	FGoalData& Data = GoalData[TargetGoal];

	if (Data.bMainGoalAchieved) return false; // 이미 달성된 MainGoal일 때
	
	if (SubGoalIndex == -1) // SubGoal이 없는 Goal
	{
		FString Str = "MainGoal Achieved : " + FString::FromInt(TargetGoal);
		UC_Util::Print(Str, FColor::Red, 10.f);
		MainGoalAchievedCheckingRoutine(Data); // MainGoalAchieved checking 일련 과정 수행
		return true;
	}
	
	if (SubGoalIndex >= Data.SubGoalsAchieved.Num()) return false;

	// 이미 달성한 SubGoal일 때
	if (Data.SubGoalsAchieved[SubGoalIndex]) return false;

	// 해당 SubGoal 달성 처리
	Data.SubGoalsAchieved[SubGoalIndex] = true;

	for (bool SubGoalAchieved : Data.SubGoalsAchieved)
		if (!SubGoalAchieved) return true; // 아직 남아있는 SubGoal이 있을 떄

	// SubGoal 모두 달성, MainGoalAchieved checking 일련의 과정 수행
	FString Str = "SubGoal All achieved -> MainGoal Achieved : " + FString::FromInt(TargetGoal);
	UC_Util::Print(Str, FColor::Red, 10.f);
	
	MainGoalAchievedCheckingRoutine(Data);
	return true;
}

void UC_TutorialStageChecker::MainGoalAchievedCheckingRoutine(FGoalData& TargetData)
{
	// MainGoal 달성 처리
	TargetData.bMainGoalAchieved = true;
		
	// 남아있는 MainGoal이 있을 때 다음 MainGoal focus 처리
	UC_TutorialGoalWidget* GoalWidget = OwnerTutorialManager->GetTutorialWidget()->GetCurrentTutorialGoalWidget();
	UC_TutorialGoalExplanationContainer* GoalExplanation = OwnerTutorialManager->GetTutorialWidget()->GetCurrentStageGoalExplanationContainer();

	// 해당 MainGoal UI Succeeded 처리 표시
	if (GoalWidget)
	{
		// Focused 활성화 도중 성공 처리되었다면, Animation이 겹쳐서 이상하게 나옴, 미연의 상황을 방지하기 위해 FocusedAnimation Stop 처리
		GoalWidget->StopFocusedAnimation(CurrentMainGoalIndex);
		GoalWidget->PlaySucceededAnimation(CurrentMainGoalIndex);
	}

	// 해당 MainGoal용 TriggerBox가 존재한다면 Disabled 처리
	if (CurrentMainGoalIndex < GoalData.Num() && IsValid(GoalTriggerBoxes[CurrentMainGoalIndex]))
	{
		FString Str = "MainGoal " + FString::FromInt(CurrentMainGoalIndex) + " Achieved and Setting Current GoalTriggerBox to false";
		UC_Util::Print(Str, FColor::Red, 10.f);
		// 이미 해당 GoalTriggerBox에 도착해 있을 때, 바로 Goal 달성 처리되면서 TriangleWidget이 안꺼지는 버그가 있음
		// HiddenInGame을 바로 처리하면 버그가 있는 것으로 추정, 따라서 Pending 처리로 수정
		FTimerHandle& TimerHandle = GAMESCENE_MANAGER->GetTimerHandle();
		uint8 CapturedIndex = CurrentMainGoalIndex;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, CapturedIndex]()
		{
			GoalTriggerBoxes[CapturedIndex]->ToggleTriggerBox(false);
		}, 0.1f, false);
	}
	
	++CurrentMainGoalIndex;

	// 아직 남아있는 MainGoal이 있을 때
	if (CurrentMainGoalIndex < GoalData.Num())
	{
		// 다음 Goal Focus 처리 표시
		if (GoalWidget) GoalWidget->PlayFocusedAnimation(CurrentMainGoalIndex);

		// 다음 Goal Explanation 표시
		if (GoalExplanation)
			GoalExplanation->StartTargetGoalExplanation(CurrentMainGoalIndex);

		// 다음 Goal 전용 TriggerBox가 있다면, Enabled 처리
		if (CurrentMainGoalIndex < GoalData.Num() && IsValid(GoalTriggerBoxes[CurrentMainGoalIndex]))
		{
			FString Str = FString::FromInt(CurrentMainGoalIndex) + " Goal's Trigger Box set to true";
			UC_Util::Print(Str, FColor::Red, 10.f);
			GoalTriggerBoxes[CurrentMainGoalIndex]->ToggleTriggerBox(true);
		}
		return;
	}
	
	// 현재의 세부 Tutorial에서 모든 Main Goal을 달성함
	if (GoalExplanation) GoalExplanation->StartStageEndExplanation();
	
	OwnerTutorialManager->SetStageToNextStage();
}

void UC_TutorialStageChecker::OnStartTriggerBoxBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!Cast<AC_Player>(OtherActor)) return; // 오로지 Player만 StartTriggerBox 사용
	
	UC_Util::Print("On Tutorial Stage Start TriggerBox Triggered", FColor::Red, 10.f);
	
	// Tutorial Stage 개요 보이기
	OwnerTutorialManager->GetTutorialWidget()->SetStageExplanationPanel(TutorialStage);
	OwnerTutorialManager->GetTutorialWidget()->ToggleStageExplanationPanel(true, bShowStageExplanationVideo);

	// 이전 Stage가 존재했다면, 이전 Stage의 GoalWidget과 Goal Explanation Panel 감추기
	int Prev = static_cast<int>(TutorialStage) - 1; 
	if (Prev >= 0)
	{
		ETutorialStage PrevStage = static_cast<ETutorialStage>(Prev);
		UC_TutorialWidget* TutorialWidget = OwnerTutorialManager->GetTutorialWidget(); 
		TutorialWidget->StopTutorialGoalExplanation(PrevStage);
		TutorialWidget->GetTutorialGoalWidget(PrevStage)->SetVisibility(ESlateVisibility::Hidden);
	}
		

	// 해당 Trigger 끄기
	if (AC_TutorialStageTriggerBox* TutorialStageTriggerBox = Cast<AC_TutorialStageTriggerBox>(OverlappedActor))
		TutorialStageTriggerBox->ToggleTriggerBox(false);
}

void UC_TutorialStageChecker::InitStage()
{
	UC_Util::Print("InitStage", FColor::Red, 10.f);

	CurrentMainGoalIndex = 0;

	// Init Goal data
	for (FGoalData& Data : GoalData)
	{
		Data.bMainGoalAchieved = false;
		
		for (bool& SubGoalAchieved : Data.SubGoalsAchieved)
			SubGoalAchieved = false;
	}

	//  GoalWidget Stage Start 처리
	// StageStartAnimation 재생 1초 이 후, 첫 번째 Goal Focused Anim 처리
	UC_TutorialGoalWidget* GoalWidget = OwnerTutorialManager->GetTutorialWidget()->GetCurrentTutorialGoalWidget();
	
	if (GoalWidget)
	{
		GoalWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		GoalWidget->InitTutorialGoalWidgetAnimations();
		
		GoalWidget->PlayStageStartAnimation();
		
		FTimerHandle& TimerHandle = GAMESCENE_MANAGER->GetTimerHandle();
		GetWorld()->GetTimerManager().SetTimer
		(
			TimerHandle,
			[this, GoalWidget]()
			{
				// 1초의 텀 사이에 바로 첫 Goal을 달성했을 수도 있음, 따라서 현재 목표가 첫 번째 순서의 Goal인지 확인해서 처리함
				if (CurrentMainGoalIndex == 0)
					GoalWidget->PlayFocusedAnimation(0);
			},
		1.f, false);
	}

	// 첫 Goal Explanation 시작
	OwnerTutorialManager->GetTutorialWidget()->StartTutorialGoalExplanation(OwnerTutorialManager->GetCurrentStage(), 0);
	
	// 초기 clear 처리
	ClearSubscribedDelegates();
	
	// 나머지 Delegate 구독 구현부는 자식 단계에서 처리
}

void UC_TutorialStageChecker::ClearSubscribedDelegates()
{
	for (auto Delegate : SubscribedDelegates) Delegate.Unbind();
	SubscribedDelegates.Empty();

	for (auto Delegate : SubscribedWidgetNumberUpdaterDelegates)
		Delegate.Clear();

	SubscribedWidgetNumberUpdaterDelegates.Empty();
}
