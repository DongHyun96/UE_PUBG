// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TutorialGoalWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
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

		// Init Widgets
		
		/*FString FocusedBackgroundImageName = FString::Printf(TEXT("FocusedBackgroundImage%d"), index);
		UImage* FocusedBackgroundImage = Cast<UImage>(GetWidgetFromName(*FocusedBackgroundImageName));
		if (!IsValid(FocusedBackgroundImage))
		{
			UC_Util::Print("From TutorialGoalWidget : Right index but cannot find FocusedBackgroundImage!", FColor::Red, 10.f);
			break;
		}
		FocusedBackgroundImages.Add(FocusedBackgroundImage);

		FString CheckBoxesName = FString::Printf(TEXT("CheckBoxes%d"), index);
		UOverlay* CheckBox = Cast<UOverlay>(GetWidgetFromName(*CheckBoxesName));
		if (!IsValid(CheckBox))
		{
			UC_Util::Print("From TutorialGoalWidget : Right index but cannot find CheckBoxes!", FColor::Red, 10.f);
			break;
		}
		CheckBoxes.Add(CheckBox);

		FString CompletedWhiteImageName = FString::Printf(TEXT("CompletedWhiteImage%d"), index);
		UImage* CompletedWhiteImage = Cast<UImage>(GetWidgetFromName(*CompletedWhiteImageName));
		if (!IsValid(CompletedWhiteImage))
		{
			UC_Util::Print("From TutorialGoalWidget : Right index but cannot find CompletedWhiteImage!", FColor::Red, 10.f);
			break;
		}
		CompletedWhiteImages.Add(CompletedWhiteImage);
		
		FString CompletedBoxImageName = FString::Printf(TEXT("CompletedBoxImage%d"), index);
		UImage* CompletedBoxImage = Cast<UImage>(GetWidgetFromName(*CompletedBoxImageName));
		if (!IsValid(CompletedBoxImage))
		{
			UC_Util::Print("From TutorialGoalWidget : Right index but cannot find CompletedBoxImage!", FColor::Red, 10.f);
			break;
		}
		
		CompletedBoxImages.Add(CompletedBoxImage);

		FString CheckedBoxImageName = FString::Printf(TEXT("CheckedBoxImage%d"), index);
		UImage* CheckedBoxImage = Cast<UImage>(GetWidgetFromName(*CheckedBoxImageName));
		if (!IsValid(CheckedBoxImage))
		{
			UC_Util::Print("From TutorialGoalWidget : Right index but cannot find CheckedBoxImage!", FColor::Red, 10.f);
			break;
		}
		CheckedBoxImages.Add(CheckedBoxImage);

		FString CheckedBoxEffectImageName = FString::Printf(TEXT("CheckedBoxEffectImage%d"), index);
		UImage* CheckedBoxEffectImage = Cast<UImage>(GetWidgetFromName(*CheckedBoxEffectImageName));
		if (!IsValid(CheckedBoxEffectImage))
		{
			UC_Util::Print("From TutorialGoalWidget : Right index but cannot find CheckedBoxEffectImage!", FColor::Red, 10.f);
			break;
		}
		CheckedBoxEffectImages.Add(CheckedBoxEffectImage);

		FString OverlayName = FString::Printf(TEXT("Overlay%d"), index);
		UOverlay* Overlay = Cast<UOverlay>(GetWidgetFromName(*OverlayName));
		if (!IsValid(Overlay))
		{
			UC_Util::Print("From TutorialGoalWidget : Right index but cannot find Overlay!", FColor::Red, 10.f);
			break;
		}
		Overlays.Add(Overlay);*/
		
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
