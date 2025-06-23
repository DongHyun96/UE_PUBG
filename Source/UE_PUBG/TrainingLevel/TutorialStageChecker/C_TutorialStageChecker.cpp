// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TutorialStageChecker.h"

#include "Character/C_Player.h"
#include "Components/BoxComponent.h"
#include "Utility/C_Util.h"

UC_TutorialStageChecker::UC_TutorialStageChecker()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_TutorialStageChecker::BeginPlay()
{
	Super::BeginPlay();
}

void UC_TutorialStageChecker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UC_TutorialStageChecker::OnGoalAchieved(uint8 TargetGoal, int SubGoalIndex)
{
	if (TargetGoal >= GoalData.Num()) return false;
	FGoalData& Data = GoalData[TargetGoal];

	if (Data.bMainGoalAchieved) return false; // 이미 달성된 MainGoal일 때
	
	if (SubGoalIndex == -1) // SubGoal이 없는 Goal
		return MainGoalAchievedCheckingRoutine(Data); // MainGoalAchieved checking 일련 과정 수행
	
	if (SubGoalIndex >= Data.SubGoalsAchieved.Num()) return false;

	// 이미 달성한 SubGoal일 때
	if (Data.SubGoalsAchieved[SubGoalIndex]) return false;

	// 해당 SubGoal 달성 처리
	Data.SubGoalsAchieved[SubGoalIndex] = true;

	for (bool SubGoalAchieved : Data.SubGoalsAchieved)
		if (!SubGoalAchieved) return true; // 아직 남아있는 SubGoal이 있을 떄

	// SubGoal 모두 달성, MainGoalAchieved checking 일련의 과정 수행
	return MainGoalAchievedCheckingRoutine(Data);
}

bool UC_TutorialStageChecker::MainGoalAchievedCheckingRoutine(FGoalData& TargetData)
{
	// MainGoal 달성 처리
	TargetData.bMainGoalAchieved = true; 
		
	// TODO : 해당되는 MainGoal UI 업데이트 (체크표시)

	for (const FGoalData& GData : GoalData) // 아직 남아있는 MainGoal이 있을 때
		if (!GData.bMainGoalAchieved) return true;
	
	// 현재의 세부 Tutorial에서 모든 Main Goal을 달성함
	// TODO : 다음 문 열기, Delegate 모두 해제
	
	return true;
}

void UC_TutorialStageChecker::OnStartTriggerBoxBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (bHasStart) return;
	if (!Cast<AC_Player>(OtherActor)) return; // 오로지 Player만 StartTriggerBox 사용
	
	bHasStart = true;
	UC_Util::Print("Triggered", FColor::Red, 10.f);
	
	// TODO : Tutorial 개요 UI 보여주기 & Player Input 잠시 꺼두고 UI 쪽(SpaceBar skip)에서만 사용
}
