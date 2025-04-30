// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_GameOverWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_GameOverWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:

	void SetOwnerPlayer(class AC_Player* InOwnerPlayer) { OwnerPlayer = InOwnerPlayer; }

private:

	AC_Player* OwnerPlayer{};

protected:

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* WinningChickenAnimation{};
	
};
