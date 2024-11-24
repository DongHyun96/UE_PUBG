// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_InstructionWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_InstructionWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
	void ToggleEjectInstructionVisibility(bool Visible);

	UFUNCTION(BlueprintImplementableEvent)
	void ToggleDeployParachuteInstructionVisibility(bool Visible);
	
};
