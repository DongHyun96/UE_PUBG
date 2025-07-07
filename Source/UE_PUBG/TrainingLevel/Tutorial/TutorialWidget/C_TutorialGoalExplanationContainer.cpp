// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TutorialGoalExplanationContainer.h"

#include "C_TutorialGoalExplanation.h"
#include "Utility/C_Util.h"

void UC_TutorialGoalExplanationContainer::NativeConstruct()
{
	Super::NativeConstruct();

	for (int i = 0;; ++i)
	{
		FString ExplanationWidgetStr = "GoalExplanation" + FString::FromInt(i);
		FName WidgetName(*ExplanationWidgetStr);
		
		if (UC_TutorialGoalExplanation* TutorialGoalExplanation = Cast<UC_TutorialGoalExplanation>(GetWidgetFromName(WidgetName)))
		{
			TutorialGoalExplanation->StopExplanation(); // 처음에 Visibility Hidden 처리하기 위함
			TutorialGoalExplanations.Add(TutorialGoalExplanation);
		}
		else break;
	}

	// Init GoalEnd Explanation
	StageEndExplanation = Cast<UC_TutorialGoalExplanation>(GetWidgetFromName("StageEndExplanation"));
	
	if (!StageEndExplanation)
		UC_Util::Print
		(
			"From UC_TutorialGoalExplanationContainer::NativeConstruct : Cannot find StageEndExplanation!",
			FColor::Red,
			10.f
		);
}

bool UC_TutorialGoalExplanationContainer::StartTargetGoalExplanation(uint8 TargetGoalIndex)
{
	if (TargetGoalIndex >= TutorialGoalExplanations.Num())
	{
		UC_Util::Print("From UC_TutorialGoalExplanationContainer::StartTargetGoalExplanation : Invalid Goal Index received!", FColor::Red, 10.f);
		return false;
	}

	this->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	// 그 이전 Goal Explanation 멈추기
	if (TargetGoalIndex != 0) TutorialGoalExplanations[TargetGoalIndex - 1]->StopExplanation();

	// 해당 GoalIndex Explanation 시작
	TutorialGoalExplanations[TargetGoalIndex]->StartExplanation();

	return true;
}

void UC_TutorialGoalExplanationContainer::StopExplanations()
{
	this->SetVisibility(ESlateVisibility::Hidden);
}

void UC_TutorialGoalExplanationContainer::StartStageEndExplanation()
{
	// 이전 TutorialGoal 문구 모두 멈추기
	for (UC_TutorialGoalExplanation* Explanation : TutorialGoalExplanations)
		Explanation->StopExplanation();

	UC_Util::Print("Start Stage End Explanation", FColor::Red, 10.f);

	if (StageEndExplanation) StageEndExplanation->StartExplanation();
}
