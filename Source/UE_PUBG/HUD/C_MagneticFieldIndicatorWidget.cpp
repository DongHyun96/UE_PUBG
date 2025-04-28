// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/C_MagneticFieldIndicatorWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UC_MagneticFieldIndicatorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Warning Triangle init 관련
	WarningTrianglePanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("WarningTrianglePanel")));
	WarningTriangleExclamationMark = Cast<UTextBlock>(GetWidgetFromName(TEXT("ExclamationMark")));

	// Timer Text 관련
	TimerText		= Cast<UTextBlock>(GetWidgetFromName(TEXT("TimerText")));
	TimerTextSlot	= Cast<UCanvasPanelSlot>(TimerText->Slot);

	// Phase Text
	PhaseText = Cast<UTextBlock>(GetWidgetFromName(TEXT("PhaseText")));

	ProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ShrinkProgressBar")));
	
}

void UC_MagneticFieldIndicatorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	HandleWarningTriangleOpacity(InDeltaTime);
	HandleExclamationMarkFlicker(InDeltaTime);
	HandleTimerTextOffsetLerp(InDeltaTime);
	HandleProgressBarPercent(InDeltaTime);
}

void UC_MagneticFieldIndicatorWidget::HandleWarningTriangleOpacity(float DeltaTime)
{
	float Opacity = WarningTrianglePanel->GetRenderOpacity();
	Opacity = FMath::Lerp(Opacity, WarningTrianglePanelLerpAlphaDest, DeltaTime * 10.f);
	WarningTrianglePanel->SetRenderOpacity(Opacity);
}

void UC_MagneticFieldIndicatorWidget::HandleExclamationMarkFlicker(float DeltaTime)
{
	if (WarningTrianglePanel->GetRenderOpacity() < 0.1f) return;

	static float TimeAccumulator{};
	TimeAccumulator += DeltaTime;

	static const float MinAlpha     = 0.1f;
	static const float MaxAlpha     = 1.f;
	static const float FlickerSpeed = 5.f;

	float Alpha = MinAlpha + (MaxAlpha - MinAlpha) * (0.5f + 0.5f * FMath::Sin(FlickerSpeed * TimeAccumulator));
	WarningTriangleExclamationMark->SetRenderOpacity(Alpha);
}

void UC_MagneticFieldIndicatorWidget::HandleTimerTextOffsetLerp(float DeltaTime)
{
	float LeftOffset = FMath::Lerp(TimerTextSlot->GetOffsets().Left, TimerTextLeftOffsetLerpDestination, DeltaTime * 10.f);
	TimerTextSlot->SetOffsets(FMargin(LeftOffset, 0.f, 0.f, 0.f));
}

void UC_MagneticFieldIndicatorWidget::HandleProgressBarPercent(float DeltaTime)
{
	float Progress = FMath::Lerp(ProgressBar->GetPercent(), ProgressBarPercentLerpDestination, DeltaTime * 10.f);
	ProgressBar->SetPercent(Progress);
}

void UC_MagneticFieldIndicatorWidget::ToggleWarningTriangleVisibility(bool Visible)
{
	if (Visible)
	{
		WarningTrianglePanelLerpAlphaDest = 1.f;
		TimerTextLeftOffsetLerpDestination = TimerTextLeftOffsetWhenWarningTriangleVisible;
	}
	else
	{
		WarningTrianglePanelLerpAlphaDest = 0.f;
		TimerTextLeftOffsetLerpDestination = 0.f;
	}
}

void UC_MagneticFieldIndicatorWidget::SetTimerText(float TimerSeconds)
{
	TimerSeconds += 1.f;
	const int Minutes = TimerSeconds / 60;
	const int Seconds = TimerSeconds - Minutes * 60;

	FString DisplayText = (Minutes >= 1) ? FString::FromInt(Minutes) + " : " : "";

	if (Seconds < 10)	DisplayText += "0" + FString::FromInt(Seconds);
	else				DisplayText += FString::FromInt(Seconds); 
	
	TimerText->SetText(FText::FromString(DisplayText));
}

void UC_MagneticFieldIndicatorWidget::SetPhaseText(const FString& Text)
{
	PhaseText->SetText(FText::FromString(Text));
}

void UC_MagneticFieldIndicatorWidget::SetProgressBarPercent(float Percent)
{
	// ProgressBar->SetPercent(Percent);
	ProgressBarPercentLerpDestination = Percent;
}