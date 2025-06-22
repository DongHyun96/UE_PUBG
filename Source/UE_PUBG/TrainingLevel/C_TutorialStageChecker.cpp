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

	for (uint8 SubGoalCount : SubGoalCounts)
	{
		TArray<bool> SubGoals{};
		SubGoals.Init(false, SubGoalCount);
	}
}

void UC_TutorialStageChecker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UC_TutorialStageChecker::OnSubGoalAchieved(uint8 MainGoalIndex, uint8 SubGoalIndex)
{
	if (MainGoalIndex >= MainGoalsAchieved.Num()) return false;
	if (SubGoalIndex >= SubGoalsAchieved[MainGoalIndex].Num()) return false;

	// 이미 달성한 MainGoal이거나 SubGoal이라면 return false
	if (MainGoalsAchieved[MainGoalIndex]) return false;
	if (SubGoalsAchieved[MainGoalIndex][SubGoalIndex]) return false;

	SubGoalsAchieved[MainGoalIndex][SubGoalIndex] = true;

	for (bool SubGoalAchieved : SubGoalsAchieved[MainGoalIndex])
		if (!SubGoalAchieved) return true; // 아직 남아있는 SubGoal이 있을 떄

	// MainGoal 안의 모든 SubGoal을 달성함
	MainGoalsAchieved[MainGoalIndex] = true;
	
	// TODO : 해당되는 MainGoal UI 업데이트 (체크표시)

	for (bool MainGoalAchieved : MainGoalsAchieved) // 아직 남아있는 MainGoal이 있을 때
		if (!MainGoalAchieved) return true;
	
	// 현재의 세부 Tutorial에서 모든 Main Goal을 달성함
	// TODO : 다음 문 열기, Delegate 모두 해제
	
	return true;
}

void UC_TutorialStageChecker::OnStartTriggerBoxBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (bHasStart) return;
	if (!Cast<AC_Player>(OtherActor)) return; // 오로지 Player만 StartTriggerBox 사용
	
	bHasStart = true;
	
	// TODO : Tutorial 개요 UI 보여주기 & Player Input 잠시 꺼두고 UI 쪽(SpaceBar skip)에서만 사용 
	
}
