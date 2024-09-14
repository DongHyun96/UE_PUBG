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

	void OnMKey();

private:

	//FEventReply OnMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent) override;

private:

	bool bInputActionSettingInited{};

	
};
