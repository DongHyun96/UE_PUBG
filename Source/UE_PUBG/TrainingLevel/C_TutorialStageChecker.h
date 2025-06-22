// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_TutorialStageChecker.generated.h"

enum class ETutorialStage : uint8;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
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
	/// 각 TutorialStage에서 특정한 Tutorial Sub Gaol 행위를 완료했을 때 CallBack 받는 함수
	/// </summary>
	/// <return> 이미 Achieved 된 상태이거나, 해당하는 Goal Index가 존재하지 않다면 return false </return>
	bool OnSubGoalAchieved(uint8 MainGoalIndex, uint8 SubGoalIndex);

public:

	UFUNCTION()
	void OnStartTriggerBoxBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	
private:
	
	// 이 Component를 갖고 있는 TutorialManager
	AC_TutorialManager* OwnerTutorialManager{};

protected:

	// 세부 Tutorial 단계에서의 각 Main 목표 순서로 Achieved되었는지 체크
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<bool> MainGoalsAchieved{};

	// Main 목표순으로 SubGoalCounts가 몇개가 되는지 체크 
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<uint8> SubGoalCounts{};
	
private:
	
	TArray<TArray<bool>> SubGoalsAchieved{}; // Main Goal 순서대로 SubGoals 나열 | SubGoals Achieved 되었는지 체크
	
	bool bHasStart{}; // 현재 Stage가 TriggerBox Overlap에 의해 시작되었는지 체크  
};
