// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_MagneticFieldIndicatorWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_MagneticFieldIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

protected:
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:

	void HandleWarningTriangleOpacity(float DeltaTime);
	void HandleExclamationMarkFlicker(float DeltaTime);

	void HandleTimerTextOffsetLerp(float DeltaTime);

	void HandleProgressBarPercent(float DeltaTime);
	
public:

	void ToggleWarningTriangleVisibility(bool Visible);

	void SetTimerText(float TimerSeconds);
	void SetPhaseText(const FString& Text);

	void SetProgressBarPercent(float Percent);

private:

private: // Warning Triangle 관련

	class UCanvasPanel* WarningTrianglePanel{};
	class UTextBlock*	WarningTriangleExclamationMark{};
	float				WarningTrianglePanelLerpAlphaDest{};

private: // Timer Text 관련

	UTextBlock* 			TimerText{};
	class UCanvasPanelSlot*	TimerTextSlot{};
	float					TimerTextLeftOffsetLerpDestination{};
	const float				TimerTextLeftOffsetWhenWarningTriangleVisible = 30.f; // Warning Triangle이 나왔을 때의 TimeText Left Offset Destination
	
private:
	
	UTextBlock* PhaseText{};

private:

	class UProgressBar* ProgressBar{};
	float ProgressBarPercentLerpDestination{};
};



