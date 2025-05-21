// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_TimeBoxWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_TimeBoxWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:

	void SetAliveText(const FString& InText);
	
	/// <summary>
	/// <para> AliveCountText 세팅하기 </para>
	/// </summary>
	/// <param name="Count"></param>
	void SetAliveCountText(int InCount);

	void SetKilledCountText(int InCount);
	void ToggleKilledCountTextVisibility(bool InVisible);

protected:

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AliveCountText{};

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* AliveCountTextAnimation{};

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AliveText{};
		
protected:

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* KilledCanvasPanel{};

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KilledCountText{};


private:

	float KilledCanvasOpacityLerpDest{};

};


