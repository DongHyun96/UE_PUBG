// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_ArmorInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_ArmorInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void NativeConstruct() override;

protected:

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:

	// Level량 & Image Pairs
	TMap<uint8, class UImage*> BackPackImages{};
	TMap<uint8, class UImage*> HelmetImages{};
	TMap<uint8, class UImage*> VestImages{};

private:

	TMap<uint8, class UMaterialInstanceDynamic*> BackPackMaterialInstanceDynamics{};
	TMap<uint8, class UMaterialInstanceDynamic*> HelmetMaterialInstanceDynamics{};
	TMap<uint8, class UMaterialInstanceDynamic*> VestMaterialInstanceDynamics{};
};
