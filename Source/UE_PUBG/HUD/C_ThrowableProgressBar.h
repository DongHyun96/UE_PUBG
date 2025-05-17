// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "C_ThrowableProgressBar.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_ThrowableProgressBar : public UCanvasPanel
{
	GENERATED_BODY()
	
public:
	
	/// <summary>
	/// HUD Widget의 NativeConstruct부분에서 호출될 함수
	/// </summary>
	void OnParentWidgetNativeConstruct();
	
	/// <summary>
	/// Custom Tick (C_HUDWidget 클래스 내에서 호출될 예정) 
	/// </summary>
	void Tick(float InDeltaTime);

public:

	void SetPercent(float InPercent) { ProgressBar->SetPercent(InPercent); }

	void SetProgressBar(UProgressBar* InProgressBar) { ProgressBar = InProgressBar; }
	void SetWarningFlickerImage(class UImage* InWarningFlickerImage) { WarningFlickerImage = InWarningFlickerImage; }

private:

	UProgressBar* ProgressBar{};
	class UImage* WarningFlickerImage{};

private:

	const float WarningStartPercent = 0.68f; // WarningFlickering과 ProgressBar 색상 더 진하게 처리하는 시점

private:

	float FlickerElapsedTime{};
	const float FlickerInterval = 0.05f;
	bool bIsFlickerImageVisible = true;

};
