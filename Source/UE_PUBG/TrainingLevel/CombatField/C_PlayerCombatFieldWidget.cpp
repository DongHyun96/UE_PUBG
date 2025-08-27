// Fill out your copyright notice in the Description page of Project Settings.


#include "C_PlayerCombatFieldWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Components/CanvasPanel.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Utility/C_Util.h"

void UC_PlayerCombatFieldWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!IsValid(StartFKeyInstructionBox))
		UC_Util::Print("From UC_PlayerCombatFieldWidget::NativeConstruct : StartFKeyInstruction Widget Binding failed!", FColor::Red, 10.f);
	else StartFKeyInstructionBox->SetVisibility(ESlateVisibility::Hidden);
}

void UC_PlayerCombatFieldWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UC_PlayerCombatFieldWidget::ToggleStartFKeyInstruction(bool Visible)
{
	if (!IsValid(StartFKeyInstructionBox))
	{
		UC_Util::Print("From UC_PlayerCombatFieldWidget::ToggleStartFKeyInstruction : Invalid StartFKeyInstruction!", FColor::Red, 10.f);
		return;
	}

	StartFKeyInstructionBox->SetVisibility(Visible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
}

void UC_PlayerCombatFieldWidget::ToggleTopRoundTimerPanelVisibility(bool Visible)
{
	if (!IsValid(TopRoundTimerPanel))
	{
		UC_Util::Print("From UC_PlayerCombatFieldWidget::ToggleTopRoundTimerPanelVisibility : Invalid TopRoundTimerPanel!", FColor::Red, 10.f);
		return;
	}
	TopRoundTimerPanel->SetVisibility(Visible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);	
}

void UC_PlayerCombatFieldWidget::ExecuteRoundStart(uint8 InRoundNumber)
{
	if (!IsValid(RoundStartAnimation))
	{
		UC_Util::Print("From UC_PlayerCombatFieldWidget::ExecuteRoundStart : Invalid RoundStartAnimation", FColor::Red, 10.f);
		return;
	}

	const FString RoundText = "Round " + FString::FromInt(InRoundNumber);
	RoundStartRoundText->SetText(FText::FromString(RoundText));

	PlayAnimation(RoundStartAnimation);
}

void UC_PlayerCombatFieldWidget::SetTopRoundTimerText(float LeftRoundTime)
{
	const int TimeLeftTotalSeconds = static_cast<int>(LeftRoundTime) + 1;
	const int Minute = TimeLeftTotalSeconds / 60;
	const int Second = TimeLeftTotalSeconds % 60;

	const FString MinuteText = "0" + FString::FromInt(Minute);
	const FString SecondText = (Second > 9) ? FString::FromInt(Second) : "0" + FString::FromInt(Second);

	RoundTimeMinuteText->SetText(FText::FromString(MinuteText));
	RoundTimeSecText->SetText(FText::FromString(SecondText));
}

void UC_PlayerCombatFieldWidget::SetTopRoundTimerTextToZero()
{
	RoundTimeMinuteText->SetText(FText::FromString("00"));
	RoundTimeSecText->SetText(FText::FromString("00"));
}
