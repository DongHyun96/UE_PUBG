// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "C_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void BeginPlay() override;

	//void SetupInputComponent() override;
	//
	//void HandleTurn(float Value);
	//
	//void HandleLookUp(float Value);
};


