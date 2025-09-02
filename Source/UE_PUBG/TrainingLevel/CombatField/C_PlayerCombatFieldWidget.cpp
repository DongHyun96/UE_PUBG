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

	// Set Player names
	FString PlayerName = GAMESCENE_MANAGER->GetPlayer()->GetCharacterName();
	RoundResultPlayerNameText->SetText(FText::FromString(PlayerName));
	MatchResultPanelPlayerName->SetText(FText::FromString(PlayerName));

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
	RoundEndAnimationEndDelegate.BindDynamic(this, &UC_PlayerCombatFieldWidget::OnRoundEndAnimationFinished);

	for (int i = 1; i < RoundCompleteAnimations.Num(); ++i)
		BindToAnimationFinished(RoundCompleteAnimations[i], RoundEndAnimationEndDelegate);

	// Dummy data
	MatchResultPanelRoundResults.Add(FMatchResultPanelRoundResult());
	
	for (int i = 1; i < 4; ++i)
	{
		FMatchResultPanelRoundResult MatchResultPanelRoundResult{};

		FString PlayerResultMainString = "Round" + FString::FromInt(i) + "PlayerResultMainText";
		FString PlayerResultSubString  = "Round" + FString::FromInt(i) + "PlayerResultSubText";

		FString EnemyResultMainString = "Round" + FString::FromInt(i) + "EnemyResultMainText";
		FString EnemyResultSubString  = "Round" + FString::FromInt(i) + "EnemyResultSubText";

		FString RoundSpentMinuteString = "Round" + FString::FromInt(i) + "MinuteSpentText";
		FString RoundSpentSecondString = "Round" + FString::FromInt(i) + "SecondSpentText";

		FString PlayerRoundResultPanelString = "Round" + FString::FromInt(i) + "PlayerResultPanel";
		FString EnemyRoundResultPanelString  = "Round" + FString::FromInt(i) + "EnemyResultPanel";

		if (UTextBlock* PlayerResultMainTextBlock = Cast<UTextBlock>(GetWidgetFromName(*PlayerResultMainString)))
			MatchResultPanelRoundResult.RoundPlayerResultMainText = PlayerResultMainTextBlock;
		else UC_Util::Print("From UC_PlayerCombatFieldWidget::NativeConstruct : PlayerResultMainText init failed", FColor::Red, 10.f);

		if (UTextBlock* PlayerResultSubTextBlock = Cast<UTextBlock>(GetWidgetFromName(*PlayerResultSubString)))
			MatchResultPanelRoundResult.RoundPlayerResultSubText = PlayerResultSubTextBlock;
		else UC_Util::Print("From UC_PlayerCombatFieldWidget::NativeConstruct : PlayerResultSubText init failed", FColor::Red, 10.f);
		

		if (UTextBlock* EnemyResultMainTextBlock = Cast<UTextBlock>(GetWidgetFromName(*EnemyResultMainString)))
			MatchResultPanelRoundResult.RoundEnemyResultMainText = EnemyResultMainTextBlock;
		else UC_Util::Print("From UC_EnemyCombatFieldWidget::NativeConstruct : EnemyResultMainText init failed", FColor::Red, 10.f);

		if (UTextBlock* EnemyResultSubTextBlock = Cast<UTextBlock>(GetWidgetFromName(*EnemyResultSubString)))
			MatchResultPanelRoundResult.RoundEnemyResultSubText = EnemyResultSubTextBlock;
		else UC_Util::Print("From UC_EnemyCombatFieldWidget::NativeConstruct : EnemyResultSubText init failed", FColor::Red, 10.f);

		if (UTextBlock* MinuteTextBlock = Cast<UTextBlock>(GetWidgetFromName(*RoundSpentMinuteString)))
			MatchResultPanelRoundResult.RoundSpentMinuteText = MinuteTextBlock;
		else UC_Util::Print("From UC_EnemyCombatFieldWidget::NativeConstruct : RoundSpendMinuteText init failed", FColor::Red, 10.f);

		if (UTextBlock* SecondTextBlock = Cast<UTextBlock>(GetWidgetFromName(*RoundSpentSecondString)))
			MatchResultPanelRoundResult.RoundSpentSecondText = SecondTextBlock;
		else UC_Util::Print("From UC_EnemyCombatFieldWidget::NativeConstruct : RoundSpendSecondText init failed", FColor::Red, 10.f);

		if (UCanvasPanel* PlayerRoundResultPanel = Cast<UCanvasPanel>(GetWidgetFromName(*PlayerRoundResultPanelString)))
			MatchResultPanelRoundResult.PlayerRoundResultPanel = PlayerRoundResultPanel;
		else UC_Util::Print("From UC_EnemyCombatFieldWidget::NativeConstruct : PlayerRoundResultPanel init failed", FColor::Red, 10.f);

		if (UCanvasPanel* EnemyRoundResultPanel = Cast<UCanvasPanel>(GetWidgetFromName(*EnemyRoundResultPanelString)))
			MatchResultPanelRoundResult.EnemyRoundResultPanel = EnemyRoundResultPanel;
		else UC_Util::Print("From UC_EnemyCombatFieldWidget::NativeConstruct : EnemyRoundResultPanel init failed", FColor::Red, 10.f);
		
		MatchResultPanelRoundResults.Add(MatchResultPanelRoundResult);
	}

	/* Bind MatchEndAnimationEnd Delegate */
	MatchEndAnimationEndDelegate.BindDynamic(this, &UC_PlayerCombatFieldWidget::UC_PlayerCombatFieldWidget::OnMatchEndAnimationFinished);
	BindToAnimationFinished(MatchEndAnimation, MatchEndAnimationEndDelegate);
	
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

void UC_PlayerCombatFieldWidget::ExecuteMatchEnd
(
	TArray<FPlayerCombatRoundResult>& RoundResults,
	uint8 PlayerWinCount, uint8 EnemyWinCount
)
{
	MatchCompleteBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	MatchCompleteTextPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	
	/* 내용 Setting */
	
	MatchResultPlayerScoreText->SetText(FText::AsNumber(PlayerWinCount));
	MatchResultEnemyScoreText->SetText(FText::AsNumber(EnemyWinCount));

	FString MatchResultString = (PlayerWinCount == EnemyWinCount) ? "Draw" : (PlayerWinCount > EnemyWinCount) ? "Win" : "Defeat"; 
	MatchResultText->SetText(FText::FromString(MatchResultString));

	if (RoundResults.Num() != MatchResultPanelRoundResults.Num())
	{
		UC_Util::Print
		(
			"From UC_PlayerCombatFieldWidget::ExecuteMatchEnd : Received RoundResults Array Num not matching with Widget RoundResults Num",
			FColor::Red, 10.f
		);
		return;
	}

	for (int i = 1; i < MatchResultPanelRoundResults.Num(); ++i)
	{
		FMatchResultPanelRoundResult& RoundResultWidget = MatchResultPanelRoundResults[i];
		
		if (RoundResults[i].RoundResult == EPlayerCombatRoundResult::NotPlayed)
		{
			RoundResultWidget.PlayerRoundResultPanel->SetVisibility(ESlateVisibility::Hidden);			
			RoundResultWidget.EnemyRoundResultPanel->SetVisibility(ESlateVisibility::Hidden);

			RoundResultWidget.RoundSpentMinuteText->SetText(FText::FromString("00"));
			RoundResultWidget.RoundSpentSecondText->SetText(FText::FromString("00"));
			continue;
		}

		RoundResultWidget.PlayerRoundResultPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);			
		RoundResultWidget.EnemyRoundResultPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		const int SpentTime		= static_cast<int>(RoundResults[i].RoundPlayTime) + 1;
		const int SpentMinute 	= SpentTime / 60;
		int SpentSecond 		= SpentTime % 60;

		// 1분 1초 방지
		if (SpentMinute == 1) SpentSecond = 0;

		const FString MinuteText = "0" + FString::FromInt(SpentMinute);
		const FString SecondText = (SpentSecond > 9) ? FString::FromInt(SpentSecond) : "0" + FString::FromInt(SpentSecond);
		
		RoundResultWidget.RoundSpentMinuteText->SetText(FText::FromString(MinuteText));
		RoundResultWidget.RoundSpentSecondText->SetText(FText::FromString(SecondText));

		// FString PlayerResultMain = RoundResults[i].RoundResult == EPlayerCombatRoundResult::Draw ? "D"

		if (RoundResults[i].RoundResult == EPlayerCombatRoundResult::Draw)
		{
			RoundResultWidget.RoundPlayerResultMainText->SetText(FText::FromString("D"));
			RoundResultWidget.RoundPlayerResultSubText ->SetText(FText::FromString("DRAW"));
			RoundResultWidget.RoundEnemyResultMainText ->SetText(FText::FromString("D"));
			RoundResultWidget.RoundEnemyResultSubText  ->SetText(FText::FromString("DRAW"));
		}
		else if (RoundResults[i].RoundResult == EPlayerCombatRoundResult::PlayerWin)
		{
			RoundResultWidget.RoundPlayerResultMainText->SetText(FText::FromString("W"));
			RoundResultWidget.RoundPlayerResultSubText ->SetText(FText::FromString("WIN"));
			RoundResultWidget.RoundEnemyResultMainText ->SetText(FText::FromString("L"));
			RoundResultWidget.RoundEnemyResultSubText  ->SetText(FText::FromString("LOSE"));
		}
		else // EnemyWin
		{
			RoundResultWidget.RoundPlayerResultMainText->SetText(FText::FromString("L"));
			RoundResultWidget.RoundPlayerResultSubText ->SetText(FText::FromString("LOSE"));
			RoundResultWidget.RoundEnemyResultMainText ->SetText(FText::FromString("W"));
			RoundResultWidget.RoundEnemyResultSubText  ->SetText(FText::FromString("WIN"));
		}
	}

	PlayAnimation(MatchEndAnimation);
}

void UC_PlayerCombatFieldWidget::SetTopRoundTimerText(float LeftRoundTime)
{
	const int TimeLeftTotalSeconds = static_cast<int>(LeftRoundTime);
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

void UC_PlayerCombatFieldWidget::OnRoundEndAnimationFinished()
{
	RoundResultPanel->SetVisibility(ESlateVisibility::Hidden);
	RoundResultBackgroundBlurPanel->SetVisibility(ESlateVisibility::Hidden);

	UC_PlayerCombatFieldManager* PlayerCombatFieldManager = GAMESCENE_MANAGER->GetTrainingGroundManager()->
															GetCombatFieldManager()->GetPlayerCombatFieldManager();

	PlayerCombatFieldManager->OnRoundUIRoutineFinished();
}

void UC_PlayerCombatFieldWidget::OnMatchEndAnimationFinished()
{
	// Init PlayerCombatFieldWidget to idle(this)
	MatchCompleteTextPanel->SetVisibility(ESlateVisibility::Hidden);
	MatchCompleteBox->SetVisibility(ESlateVisibility::Hidden);

	for (int i = 1; i < 4; ++i)
	{
		TopRoundResultDotImages[i]->SetColorAndOpacity(ResultDotColors[EPlayerCombatRoundResult::Draw]);
		ResultPanelResultDotImages[i]->SetColorAndOpacity(ResultDotColors[EPlayerCombatRoundResult::Draw]);
		TopRoundResultDotImages[i]->SetRenderScale(FVector2D(1.f));
		ResultPanelResultDotImages[i]->SetRenderScale(FVector2D(1.f));
	}

	// Init PlayerCombatFieldManager
	UC_PlayerCombatFieldManager* PlayerCombatFieldManager = GAMESCENE_MANAGER->GetTrainingGroundManager()->
															GetCombatFieldManager()->GetPlayerCombatFieldManager();

	PlayerCombatFieldManager->SetPlayerCombatFieldState(EPlayerCombatFieldState::Idle);

}
