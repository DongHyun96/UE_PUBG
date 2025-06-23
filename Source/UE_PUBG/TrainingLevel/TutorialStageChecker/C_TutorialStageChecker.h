// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_TutorialStageChecker.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FGoalAchievedDelegate, uint8 /*TargetGoal*/, int /*SubGoalIndex*/);

struct FGoalData
{
	bool bMainGoalAchieved{};
	TArray<bool> SubGoalsAchieved{};
};

enum class ETutorialStage : uint8;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_PUBG_API UC_TutorialStageChecker : public UActorComponent
{
	GENERATED_BODY()

public:
	UC_TutorialStageChecker();
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetOwnerTutorialManager(class AC_TutorialManager* InOwnerTutorialManager) { OwnerTutorialManager = InOwnerTutorialManager; }

	/// <summary>
	/// 각 TutorialStage에서 특정한 Tutorial Goal 행위를 완료했을 때 CallBack 받는 함수
	/// </summary>
	/// <param name="TargetGoal"> : Target MainGoal Index </param>
	/// <param name="SubGoalIndex"> : Target SubGoal Index (만약 SubGoal이 따로 없는 MainGoal이라면 -1 사용(default value))</param>
	/// <returns> 이미 Achieved 된 상태이거나, 해당하는 Goal Index가 존재하지 않다면 return false </returns>
	bool OnGoalAchieved(uint8 TargetGoal, int SubGoalIndex = -1);

private:
	
	/// <summary>
	/// OnGoalAchieved 함수 내의 MainGoal Achieved 체크 처리 boilerplate code 정리 
	/// </summary>
	/// <param name="TargetData"></param>
	/// <returns></returns>
	bool MainGoalAchievedCheckingRoutine(FGoalData& TargetData);

public:

	UFUNCTION()
	void OnStartTriggerBoxBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	
private:
	
	// 이 Component를 갖고 있는 TutorialManager
	AC_TutorialManager* OwnerTutorialManager{};
	bool bHasStart{}; // 현재 Stage가 TriggerBox Overlap에 의해 시작되었는지 체크

protected:
	
	TArray<FGoalData> GoalData{};

private:

	// Delegate를 걸어둔 모든 Delegate들
	TArray<TMulticastDelegate<void(uint8, int)>> SubscribedDelegates{};
	
};
