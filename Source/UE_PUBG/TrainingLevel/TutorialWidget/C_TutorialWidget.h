// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_TutorialWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_TutorialWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
};
