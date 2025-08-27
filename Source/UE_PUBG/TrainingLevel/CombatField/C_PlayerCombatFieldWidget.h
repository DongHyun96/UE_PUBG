// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_PlayerCombatFieldWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_PlayerCombatFieldWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;
    
protected:
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:

	void ToggleStartFKeyInstruction(bool Visible);

	void ToggleTopRoundTimerPanelVisibility(bool Visible);

public:
	
	/// <summary>
	/// RoundStart 관련 Animation 및 내용 처리 
	/// </summary>
	/// <param name="InRoundNumber"> : 현재 시작할 라운드 수 </param>
	void ExecuteRoundStart(uint8 InRoundNumber);

public: // Top Round Timer Text 관련

	/// <summary>
	/// <para> TopRoundTimer 남은 시간 표시 </para>
	/// <para> 편의상 RoundTimer는 10분을 넘지 않는 것으로 간주하고 구현 처리함 </para>
	/// </summary>
	/// <param name="LeftRoundTime"> : Round 남은 시간 </param>
	void SetTopRoundTimerText(float LeftRoundTime);

	/// <summary>
	/// <para> 남은 초 시간 0으로 초기화 시키기 </para>
	/// <para> 남은 시간을 표기함에 있어 1을 더해서 표기해야 하기 때문에 0초는 1초로 표기되어 이 함수를 통해 0으로 표기 처리 </para>
	/// </summary>
	void SetTopRoundTimerTextToZero();

protected:

	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* StartFKeyInstructionBox{};

protected: // Top Round Timer 관련

	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* TopRoundTimerPanel{};

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* RoundTimeMinuteText{};

	UPROPERTY(meta=(BindWidget))
	UTextBlock* RoundTimeSecText{};

protected: // Round Start 관련

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* RoundStartRoundText{};
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* RoundStartAnimation{};
	
};
