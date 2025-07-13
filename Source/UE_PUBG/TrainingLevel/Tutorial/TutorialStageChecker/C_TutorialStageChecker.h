// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_TutorialStageChecker.generated.h"

// MainGoal, SubGoal 순의 파라미터를 넘겨서 처리하는 MainGoal 전용 Delegate 선언
DECLARE_DELEGATE_RetVal_TwoParams(bool, FTutorialStageGoalCheckerDelegate, uint8, int);

// Goal을 달성할 시, 특정 GoalWidget이나 Goal Explanation Widget의 특정한 숫자를 업데이트할 떄 사용
DECLARE_MULTICAST_DELEGATE_OneParam(FTutorialStageUpdateWidgetNumberDelegate, uint8);

USTRUCT(BlueprintType)
struct FGoalData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
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

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetOwnerTutorialManager(class AC_TutorialManager* InOwnerTutorialManager) { OwnerTutorialManager = InOwnerTutorialManager; }
	void SetTutorialStage(ETutorialStage InTutorialStage) { TutorialStage = InTutorialStage; }

protected:
	
	/// <summary>
	/// 각 TutorialStage에서 특정한 Tutorial Goal 행위를 완료했을 때 CallBack 받는 함수
	/// </summary>
	/// <param name="TargetGoal"> : Target MainGoal Index </param>
	/// <param name="SubGoalIndex"> : Target SubGoal Index (만약 SubGoal이 따로 없는 MainGoal이라면 -1 사용(default value))</param>
	/// <returns> 이미 Achieved 된 상태이거나, 해당하는 Goal Index가 존재하지 않다면 return false </returns>
	bool OnGoalAchieved(uint8 TargetGoal, int SubGoalIndex = -1);

private:
	
	/// <summary>
	/// OnGoalAchieved 함수 내의 MainGoal Achieved 되었을 때 일련의 체크 처리 boilerplate code 정리 
	/// </summary>
	/// <param name="TargetData"></param>
	void MainGoalAchievedCheckingRoutine(FGoalData& TargetData);

public:

	UFUNCTION()
	void OnStartTriggerBoxBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	/// <summary>
	/// <para> Stage 첫 시작 시, 해당 Tutorial init 처리 </para>
	/// <para> & 해당 Tutorial Stage에서 Gaol로 잡힌 행위들에 대한 Delegate 구독 처리 </para>
	/// <para> Init 처리 시점 - 개요 UI를 닫았을 때  </para>
	/// </summary>
	virtual void InitStage();

	/// <summary>
	/// 구독한 Delegate 모두 해제 & SubscribedDelegates Array 정리
	/// </summary>
	void ClearSubscribedDelegates(); // TODO : Tutorial 어떻게든 끝났을 때 꼭 각 TutorialStageChecker에 대해 호출해 주어야 함

protected:

	// 자기자신의 Stage 종류
	ETutorialStage TutorialStage{};
	
protected:
	
	// 이 Component를 갖고 있는 TutorialManager
	AC_TutorialManager* OwnerTutorialManager{};

private:
	
	bool bHasStart{}; // 현재 Stage가 TriggerBox Overlap에 의해 시작되었는지 체크

	// 현재 수행중인 MainGoal index - 현재 순서의 Main Goal이 아닌 미리 다음 Main Goal을 수행하여도 Sequence에 맞게끔 처리할 예정
	uint8 CurrentMainGoalIndex{};
	
protected: 

	UPROPERTY(BlueprintReadOnly)
	TArray<FGoalData> GoalData{};

protected:

	// Goal 행위 체크용 Delegate를 걸어둔 모든 Delegate들
	TArray<TDelegate<bool(uint8, int)>> SubscribedDelegates{};

	// Widget의 특정 번호를 업데이트 시켜주는 Delegate를 걸어둔 모든 Delegate들
	TArray<TMulticastDelegate<void(uint8)>> SubscribedWidgetNumberUpdaterDelegates{};
	

protected:

	// MainGoal 별 TriggerBoxes (만약에 TriggerBox가 필요없다면 nullptr / 대신 갯수는 MainGoal 개수와 동일)
	TArray<class AC_TutorialStageTriggerBox*> GoalTriggerBoxes{};
};
