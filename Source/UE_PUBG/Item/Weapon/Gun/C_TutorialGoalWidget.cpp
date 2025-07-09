// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TutorialGoalWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Utility/C_Util.h"

void UC_TutorialGoalWidget::NativeConstruct()
{
	Super::NativeConstruct();

	uint8 index{};
	while (true)
	{
		// Try find FocusedAnim with current index
		FString FocusedAnimName = FString::Printf(TEXT("GoalFocusedAnim%d"), index);

		UWidgetAnimation* FocusedAnim = UC_Util::GetWidgetAnimationByName(this, *FocusedAnimName);
		if (!IsValid(FocusedAnim)) break;

		GoalFocusedAnimations.Add(FocusedAnim);
		
		FString SucceededAnimName = FString::Printf(TEXT("GoalSucceededAnim%d"), index);

		UWidgetAnimation* SucceededAnim = UC_Util::GetWidgetAnimationByName(this, *SucceededAnimName);
		if (!IsValid(SucceededAnim)) break;

		GoalSucceededAnimations.Add(SucceededAnim);

		index++;
	}
}

void UC_TutorialGoalWidget::PlayStageStartAnimation()
{
	PlayAnimation(StageStartAnimation);
}

bool UC_TutorialGoalWidget::PlayFocusedAnimation(uint8 GoalIndex)
{
	if (GoalIndex >= GoalFocusedAnimations.Num()) return false;
	
	PlayAnimation(GoalFocusedAnimations[GoalIndex]);
	return true;
}

bool UC_TutorialGoalWidget::PlaySucceededAnimation(uint8 GoalIndex)
{
	if (GoalIndex >= GoalSucceededAnimations.Num()) return false;
	
	PlayAnimation(GoalSucceededAnimations[GoalIndex]);
	return true;
}

bool UC_TutorialGoalWidget::StopFocusedAnimation(uint8 GoalIndex)
{
	if (GoalIndex >= GoalFocusedAnimations.Num()) return false;

	StopAnimation(GoalFocusedAnimations[GoalIndex]);
	return true;
}

void UC_TutorialGoalWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}
