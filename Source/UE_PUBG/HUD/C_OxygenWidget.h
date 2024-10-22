// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_OxygenWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_OxygenWidget : public UUserWidget
{
	GENERATED_BODY()


public:

	/// <summary>
	/// Set ProgressBar Percent 
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void SetOxygenPercent(float InPercent);
	
};
