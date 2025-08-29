// Fill out your copyright notice in the Description page of Project Settings.


#include "C_PlayerCombatFieldWidget.h"

#include "C_CombatFieldManager.h"
#include "IAutomationReport.h"
#include "Animation/WidgetAnimation.h"
#include "Character/C_Player.h"
#include "Components/CanvasPanel.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Singleton/C_GameSceneManager.h"
#include "TrainingLevel/C_TrainingGroundManager.h"
#include "Utility/C_Util.h"

void UC_PlayerCombatFieldWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!IsValid(StartFKeyInstructionBox))
		UC_Util::Print("From UC_PlayerCombatFieldWidget::NativeConstruct : StartFKeyInstruction Widget Binding failed!", FColor::Red, 10.f);
	else StartFKeyInstructionBox->SetVisibility(ESlateVisibility::Hidden);

	// Init RoundResult Dot images (Index 0는 Dummy Image / 주의 : 사용하지 말 것)
	UImage* DummyImage = NewObject<UImage>(this);
	TopRoundResultDotImages.Add(DummyImage);
	ResultPanelResultDotImages.Add(DummyImage);
	
	for (int i = 1; i < 4; ++i)
	{
		FString TopRoundResultDotWidgetString = "Round" + FString::FromInt(i) + "IndicatorImage";
		
		if (UImage* Image = Cast<UImage>(GetWidgetFromName(FName(*TopRoundResultDotWidgetString))))
			TopRoundResultDotImages.Add(Image);
		else UC_Util::Print("From UC_PlayerCombatFieldWidget::NativeConstruct : " + TopRoundResultDotWidgetString + " not found!", FColor::Red, 10.f);

		FString MiddleRoundResultDotWidgetString = "ResultPanelRound" + FString::FromInt(i) + "IndicatorImage";

		if (UImage* Image = Cast<UImage>(GetWidgetFromName(FName(*MiddleRoundResultDotWidgetString))))
			ResultPanelResultDotImages.Add(Image);
		else UC_Util::Print("From UC_PlayerCombatFieldWidget::NativeConstruct : " + MiddleRoundResultDotWidgetString + " not found!", FColor::Red, 10.f);
	}

	// Set Player names // TODO : 나머지 Player 이름도 Setting을 해주어야 함
	FString PlayerName = GAMESCENE_MANAGER->GetPlayer()->GetCharacterName();
	RoundResultPlayerNameText->SetText(FText::FromString(PlayerName));

	// WidgetAnimation의 경우 BindWidgetAnim을 통하여서만 멤버변수를 초기화할 수 있기 때문에 아래처럼 초기화 시켜둠
	TopRoundResultDotAnimations.Add(Round1ResultTopDotAnimation);
	TopRoundResultDotAnimations.Add(Round1ResultTopDotAnimation);
	TopRoundResultDotAnimations.Add(Round2ResultTopDotAnimation);
	TopRoundResultDotAnimations.Add(Round3ResultTopDotAnimation);

	RoundCompleteAnimations.Add(Round1CompleteAnimation);
	RoundCompleteAnimations.Add(Round1CompleteAnimation);
	RoundCompleteAnimations.Add(Round2CompleteAnimation);
	RoundCompleteAnimations.Add(Round3CompleteAnimation);

	RoundResultPanel->SetVisibility(ESlateVisibility::Hidden);
	RoundResultBackgroundBlurPanel->SetVisibility(ESlateVisibility::Hidden);

	/* Bind RoundEndAnimationEnd Delegate */
	RoundEndAnimationEndDelegate.BindDynamic(this, &UC_PlayerCombatFieldWidget::OnRoundEndAnimationEnd);

	for (int i = 1; i < RoundCompleteAnimations.Num(); ++i)
		BindToAnimationFinished(RoundCompleteAnimations[i], RoundEndAnimationEndDelegate);
}

void UC_PlayerCombatFieldWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UC_PlayerCombatFieldWidget::ToggleStartFKeyInstruction(bool Visible)
{
	if (!IsValid(StartFKeyInstructionBox))
	{
		UC_Util::Print("From UC_PlayerCombatFieldWidget::ToggleStartFKeyInstruction : Invalid StartFKeyInstruction!", FColor::Red, 10.f);
		return;
	}

	StartFKeyInstructionBox->SetVisibility(Visible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
}

void UC_PlayerCombatFieldWidget::ToggleTopRoundTimerPanelVisibility(bool Visible)
{
	if (!IsValid(TopRoundTimerPanel))
	{
		UC_Util::Print("From UC_PlayerCombatFieldWidget::ToggleTopRoundTimerPanelVisibility : Invalid TopRoundTimerPanel!", FColor::Red, 10.f);
		return;
	}
	TopRoundTimerPanel->SetVisibility(Visible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);	
}

void UC_PlayerCombatFieldWidget::ExecuteRoundStart(uint8 InRoundNumber)
{
	if (!IsValid(RoundStartAnimation))
	{
		UC_Util::Print("From UC_PlayerCombatFieldWidget::ExecuteRoundStart : Invalid RoundStartAnimation", FColor::Red, 10.f);
		return;
	}

	RoundResultPanel->SetVisibility(ESlateVisibility::Hidden);

	// Round Result Dot 크기 조정
	for (int i = 1; i < InRoundNumber; ++i)
		TopRoundResultDotImages[i]->SetRenderScale(FVector2D(1.5f));

	for (int i = InRoundNumber; i < TopRoundResultDotImages.Num(); ++i)
		TopRoundResultDotImages[i]->SetRenderScale(FVector2D(1.f));

	const FString RoundText = "Round " + FString::FromInt(InRoundNumber);
	RoundStartRoundText->SetText(FText::FromString(RoundText));

	PlayAnimation(RoundStartAnimation);
}

void UC_PlayerCombatFieldWidget::ExecuteRoundEnd
(
	EPlayerCombatRoundResult InRoundResult,
	uint8 InCurrentRoundNumber,
	uint8 PlayerWinCount, uint8 EnemyWinCount
)
{
	/* 내용 Settings */
	RoundResultWinLoseBlurImage->SetColorAndOpacity(RoundResultPanelBGBlurColors[InRoundResult]);

	RoundResultRoundText->SetText(FText::FromString("Round " + FString::FromInt(InCurrentRoundNumber)));
	
	static const TMap<EPlayerCombatRoundResult, FString> RoundWinLoseTexts =
	{
		{EPlayerCombatRoundResult::Draw,		"DRAW"},
		{EPlayerCombatRoundResult::PlayerWin,	"WIN"},
		{EPlayerCombatRoundResult::EnemyWin,	"LOSE"},
		{EPlayerCombatRoundResult::NotPlayed,	"NOTPLAYED"}
	};
	
	RoundResultRoundWinLoseText->SetText(FText::FromString(RoundWinLoseTexts[InRoundResult]));
		
	// 현재 총 스코어 settings
	RoundResultPlayerWinCountText->SetText(FText::AsNumber(PlayerWinCount));
	RoundResultEnemyWinCountText->SetText(FText::AsNumber(EnemyWinCount));

	if (InRoundResult == EPlayerCombatRoundResult::Draw)
	{
		RoundResultPlayerWinCountBar->SetVisibility(ESlateVisibility::Hidden);
		RoundResultEnemyWinCountBar->SetVisibility(ESlateVisibility::Hidden);
	}
	else if (InRoundResult == EPlayerCombatRoundResult::PlayerWin)
	{
		RoundResultPlayerWinCountBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		RoundResultEnemyWinCountBar->SetVisibility(ESlateVisibility::Hidden);
	}
	else if (InRoundResult == EPlayerCombatRoundResult::EnemyWin)
	{
		RoundResultPlayerWinCountBar->SetVisibility(ESlateVisibility::Hidden);
		RoundResultEnemyWinCountBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	// Round Result dot settings
	TopRoundResultDotImages[InCurrentRoundNumber]->SetColorAndOpacity(ResultDotColors[InRoundResult]);
	ResultPanelResultDotImages[InCurrentRoundNumber]->SetColorAndOpacity(ResultDotColors[InRoundResult]);

	// Round result dot 크기 조정
	for (int i = 1; i < InCurrentRoundNumber; ++i)
		ResultPanelResultDotImages[i]->SetRenderScale(FVector2D(1.5f));

	for (int i = InCurrentRoundNumber + 1; i < ResultPanelResultDotImages.Num(); ++i)
		ResultPanelResultDotImages[i]->SetRenderScale(FVector2D(1.f));
	

	/* Animation playing */
	PlayAnimation(TopRoundResultDotAnimations[InCurrentRoundNumber]);

	FTimerHandle& TimerHandle = GAMESCENE_MANAGER->GetTimerHandle();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, InCurrentRoundNumber]()
	{
		RoundResultPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		RoundResultBackgroundBlurPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		
		PlayAnimation(RoundCompleteAnimations[InCurrentRoundNumber]);
	}, 2.f, false);
}

void UC_PlayerCombatFieldWidget::SetTopRoundTimerText(float LeftRoundTime)
{
	const int TimeLeftTotalSeconds = static_cast<int>(LeftRoundTime) + 1;
	const int Minute = TimeLeftTotalSeconds / 60;
	const int Second = TimeLeftTotalSeconds % 60;

	const FString MinuteText = "0" + FString::FromInt(Minute);
	const FString SecondText = (Second > 9) ? FString::FromInt(Second) : "0" + FString::FromInt(Second);

	RoundTimeMinuteText->SetText(FText::FromString(MinuteText));
	RoundTimeSecText->SetText(FText::FromString(SecondText));
}

void UC_PlayerCombatFieldWidget::SetTopRoundTimerTextToZero()
{
	RoundTimeMinuteText->SetText(FText::FromString("00"));
	RoundTimeSecText->SetText(FText::FromString("00"));
}

void UC_PlayerCombatFieldWidget::OnRoundEndAnimationEnd()
{
	RoundResultPanel->SetVisibility(ESlateVisibility::Hidden);
	RoundResultBackgroundBlurPanel->SetVisibility(ESlateVisibility::Hidden);

	UC_PlayerCombatFieldManager* PlayerCombatFieldManager = GAMESCENE_MANAGER->GetTrainingGroundManager()->
															GetCombatFieldManager()->GetPlayerCombatFieldManager();

	PlayerCombatFieldManager->OnRoundUIRoutineFinished();
}
