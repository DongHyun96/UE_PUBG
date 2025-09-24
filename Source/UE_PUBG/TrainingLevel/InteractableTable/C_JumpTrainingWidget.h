// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_JumpTrainingWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_JumpTrainingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void SetSpectatorInfoVisibility(bool Visible) { SpectatingInfoPanelOpacityDestination = Visible ? 1.f : 0.f; }

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	
	/// <summary>
	/// SpectatorInfo 패널 및 TextBlock Flickering 처리 
	/// </summary>
	void HandleSpectatorInfo(float InDeltaTime);

protected:
	
	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* SpectatingInfoPanel{};

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* SpectatorInfoText{};

private:
	
	float SpectatingInfoPanelOpacityDestination{};
};
