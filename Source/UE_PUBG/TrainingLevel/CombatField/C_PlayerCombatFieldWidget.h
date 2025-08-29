// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "C_PlayerCombatFieldManager.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "C_PlayerCombatFieldWidget.generated.h"

enum class EPlayerCombatRoundResult : uint8;
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

	void SetTopRoundText(uint8 InRoundNumber) { TopRoundText->SetText(FText::FromString("Round " + FString::FromInt(InRoundNumber))); }
	
	/// <summary>
	/// RoundStart 관련 Animation 및 내용 처리 
	/// </summary>
	/// <param name="InRoundNumber"> : 현재 시작할 라운드 수 </param>
	void ExecuteRoundStart(uint8 InRoundNumber);

	void ExecuteRoundEnd(EPlayerCombatRoundResult InRoundResult, uint8 InCurrentRoundNumber, uint8 PlayerWinCount, uint8 EnemyWinCount);

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

private:
	
	/// <summary>
	/// Round End Animation이 모두 끝난 뒤 호출될 함수, 다음 Round로 진행 또는 Match End 처리 
	/// </summary>
	UFUNCTION()
	void OnRoundEndAnimationEnd();

protected:

	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* StartFKeyInstructionBox{};

protected: // Top Round Timer 쪽 관련

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TopRoundText{};
	
	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* TopRoundTimerPanel{};

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* RoundTimeMinuteText{};

	UPROPERTY(meta=(BindWidget))
	UTextBlock* RoundTimeSecText{};

private:
	
	// RoundResult Dot / 각 Round에 맞게 Indexing / 주의 : Index 0는 Dummy 사용하지 말 것 
	TArray<class UImage*> TopRoundResultDotImages{};
	
protected: // Round Start 관련

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* RoundStartRoundText{};
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* RoundStartAnimation{};

private: // Round End 관련

	// Round Result Dot image / 각 Round에 맞게 Indexing / 주의 : Index 0는 Dummy 사용하지 말 것
	TArray<UImage*> ResultPanelResultDotImages{};

protected:
	
	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* RoundResultPanel{};

	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* RoundResultBackgroundBlurPanel{};
	
	UPROPERTY(meta=(BindWidget))
	UImage* RoundResultWinLoseBlurImage{}; // Background Blur 색상 image
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* RoundResultRoundText{}; // Round 1, 2, 3

	UPROPERTY(meta=(BindWidget))
	UTextBlock* RoundResultRoundWinLoseText{}; // Middle Round Result Text (WIN/LOSE/DRAW)

	UPROPERTY(meta=(BindWidget))
	UTextBlock* RoundResultPlayerNameText{}; // Round Result의 Player Name Text

	UPROPERTY(meta=(BindWidget))
	UTextBlock* RoundResultPlayerWinCountText{}; // Round Result의 Player Win count text
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* RoundResultEnemyWinCountText{}; // Round Result의 Enemy Win count text

	UPROPERTY(meta=(BindWidget))
	UImage* RoundResultPlayerWinCountBar{}; // Player Win Count Text 하단의 Bar Image

	UPROPERTY(meta=(BindWidget))
	UImage* RoundResultEnemyWinCountBar{}; // Enemy Win Count Text 하단의 Bar Image
	
private:

	const TMap<EPlayerCombatRoundResult, FLinearColor> ResultDotColors =
	{
		{EPlayerCombatRoundResult::Draw,		FLinearColor(0.f, 0.f, 0.f, 0.5f)},
		{EPlayerCombatRoundResult::PlayerWin,	FLinearColor(0.f, 0.f, 1.f, 0.5f)},
		{EPlayerCombatRoundResult::EnemyWin,	FLinearColor(1.f, 0.f, 0.f, 0.5f)},
		{EPlayerCombatRoundResult::NotPlayed,	FLinearColor(0.f, 0.f, 0.f, 0.5f)}
	};

	const TMap<EPlayerCombatRoundResult, FLinearColor> RoundResultPanelBGBlurColors =
	{
		{EPlayerCombatRoundResult::Draw,		FLinearColor(0.f, 0.f, 0.f, 0.5f)},
		{EPlayerCombatRoundResult::PlayerWin,	FLinearColor(0.f, 0.438498f, 1.f, 0.5f)},
		{EPlayerCombatRoundResult::EnemyWin,	FLinearColor(1.f, 0.119792f, 0.161266f, 0.5f)},
		{EPlayerCombatRoundResult::NotPlayed,	FLinearColor(0.f, 0.f, 0.f, 0.5f)}
	};

protected: // Animations

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Round1ResultTopDotAnimation{};

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Round2ResultTopDotAnimation{};

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Round3ResultTopDotAnimation{};

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Round1CompleteAnimation{};

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Round2CompleteAnimation{};

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Round3CompleteAnimation{};
	
private:

	// Index 0 dummy (Round 수에 맞추어 초기화)
	TArray<UWidgetAnimation*> TopRoundResultDotAnimations{};
	
	// Index 0 dummy (Round 수에 맞추어 초기화)
	TArray<UWidgetAnimation*> RoundCompleteAnimations{};

private:

	FWidgetAnimationDynamicEvent RoundEndAnimationEndDelegate{};
	
};
