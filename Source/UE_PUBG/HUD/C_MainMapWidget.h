// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUD/C_MapWidget.h"
#include "C_MainMapWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_MainMapWidget : public UC_MapWidget
{
	GENERATED_BODY()

public:
	void NativeConstruct() override;

	void OnMKey();

private:

	//FEventReply OnMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent) override;
	FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UImage* MainMapImg{};

private:

	float MainMapScale = 1.f;
	const float MAP_SCALE_MAX = 5.f;
	const float SCROLL_DELTA_STEP = 0.5f;
};
