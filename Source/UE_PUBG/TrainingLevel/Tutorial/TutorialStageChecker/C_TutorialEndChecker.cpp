// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TutorialEndChecker.h"

#include "Character/C_Player.h"
#include "Item/Weapon/Gun/C_TutorialGoalWidget.h"
#include "TrainingLevel/Tutorial/C_TutorialStageTriggerBox.h"
#include "TrainingLevel/Tutorial/TutorialWidget/C_TutorialWidget.h"
#include "Utility/C_Util.h"

UC_TutorialEndChecker::UC_TutorialEndChecker()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_TutorialEndChecker::BeginPlay()
{
	Super::BeginPlay();
}


void UC_TutorialEndChecker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UC_TutorialEndChecker::OnStartTriggerBoxBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	/* 얘 override는 생각보다 필요 없을듯? */
	
	if (!Cast<AC_Player>(OtherActor)) return; // 오로지 Player만 StartTriggerBox 사용
	
	UC_Util::Print("On Tutorial Stage Start TriggerBox Triggered", FColor::Red, 10.f);
	
	// Tutorial Stage 개요 보이기
	// TutorialWidget 내에서 EndTutorial만 예외처리 해주면 될 것 같음 (따라서 이 함수 override는 딱히 안해주어도 될 것 같음)
	OwnerTutorialManager->GetTutorialWidget()->SetStageExplanationPanel(TutorialStage);
	OwnerTutorialManager->GetTutorialWidget()->ToggleStageExplanationPanel(true);

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

void UC_TutorialEndChecker::InitStage()
{
	// Super::InitStage();

	UC_Util::Print("Tutorial End Stage InitStage called, Moving to Training Ground", FColor::Red, 10.f);

	// TODO : Training Ground 위치로 돌아가기 
}
