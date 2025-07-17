// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_TutorialGoalExplanationContainer.generated.h"

/// <summary>
/// Tutorial Stage 내에서 활용할 GoalExplanations Container
/// </summary>
UCLASS()
class UE_PUBG_API UC_TutorialGoalExplanationContainer : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

public:
	
	/// <summary>
	/// 해당 Index번째의 MainGoal Explanation 시작 
	/// </summary>
	/// <param name="TargetGoalIndex"> : 해당 MainGoal Index </param>
	/// <returns> : Index가 Valid하지 않다면 return false </returns>
	bool StartTargetGoalExplanation(uint8 TargetGoalIndex);

	/// <summary>
	/// 이 Stage에 해당하는 Goal Hidden 처리
	/// </summary>
	void StopExplanations();

	/// <summary>
	/// Stage Goal을 모두 달성한 이후, Stage End 문구 띄우기
	/// </summary>
	void StartStageEndExplanation();

public:
	
	/// <returns> : 해당 Index가 Valid하지 않다면 return nullptr </returns>
	class UC_TutorialGoalExplanation* GetTutorialGoalExplanation(uint8 Index) const;
	
private:

	TArray<UC_TutorialGoalExplanation*> TutorialGoalExplanations{};

	UC_TutorialGoalExplanation* StageEndExplanation{};
};
