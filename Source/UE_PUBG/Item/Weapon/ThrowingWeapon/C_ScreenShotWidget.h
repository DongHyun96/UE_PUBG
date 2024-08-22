// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_ScreenShotWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_ScreenShotWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void DisplayCapturedImage(class UTextureRenderTarget2D* RenderTarget);

public:

	void SetImageTexture(class UTexture2D* NewTexture);

	class UImage* GetDisplayImage() const { return DisplayImage; }

protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* DisplayImage{};

};
