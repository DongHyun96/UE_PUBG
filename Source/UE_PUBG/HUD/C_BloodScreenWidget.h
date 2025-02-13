// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_BloodScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BloodScreenWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	//void NavtiveTick()

	UFUNCTION(BlueprintCallable)
	void ShowHitEffect();

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DynamicOpacity = 0.0f;
};
