// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_TrainingGroundButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_TrainingGroundButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	void ToggleButtonsEnabled(bool bEnabled);

private:

	UFUNCTION()
	void OnShootingRangeButtonClicked();

	UFUNCTION()
	void OnSkyDivingFieldButtonClicked();		

	UFUNCTION()
	void OnGrenadePitButtonClicked();

	UFUNCTION()
	void OnTutorialFieldButtonClicked();

	UFUNCTION()
	void OnSupplyDepotButtonClicked();

	UFUNCTION()
	void OnGarageButtonClicked();

	UFUNCTION()
	void OnAIMovementTestFieldButtonClicked();

	UFUNCTION()
	void OnAIStatCareTestFieldButtonClicked();

	UFUNCTION()
	void OnEVETestFieldButtonClicked();
	
	UFUNCTION()
	void OnPVETestFieldButtonClicked();

private:

	// Field place buttons 저장 / ETrainingGroundPlace enum값 순서대로 Buttons에 저장 처리
	TArray<class UButton*> Buttons{};

protected:

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* TrainingGroundPlaceButtonPanel{};
	
};
