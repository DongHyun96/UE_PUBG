// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TutorialWidget.h"

#include "C_TutorialGoalExplanationContainer.h"
#include "MediaPlayer.h"
#include "Character/C_Player.h"
#include "Character/Component/C_PlayerController.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Item/Weapon/Gun/C_TutorialGoalWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Singleton/C_GameSceneManager.h"
#include "TrainingLevel/C_TutorialManager.h"
#include "Utility/C_Util.h"


void UC_TutorialWidget::NativeConstruct()
{
	Super::NativeConstruct();

	const TMap<ETutorialStage, FString> TutorialStageExplanationPanelNames =
	{
		{ETutorialStage::MovementTutorial, "MovementTutorialExplanation"},
		{ETutorialStage::MovementTutorial, "WeaponTutorialExplanation"},
		{ETutorialStage::MovementTutorial, "ThrowableTutorialExplanation"},
		{ETutorialStage::MovementTutorial, "HealingTutorialExplanation"}
	};

	for (const TTuple<ETutorialStage, FString>& PanelNameTuple : TutorialStageExplanationPanelNames)
	{
		UCanvasPanel* TargetPanel = Cast<UCanvasPanel>(GetWidgetFromName(FName(*PanelNameTuple.Value)));
		if (!TargetPanel)
		{
			UC_Util::Print("From UC_TutorialWidget::NativeConstruct : Explanation panel casting failed!", FColor::Red, 10.f);
			continue;
		}
		StageExplanations.Add(PanelNameTuple.Key, TargetPanel);
	}

	// Init TutorialGoalWidgets & TutorialGoalExplanation Widgets
	for (uint8 i = 0; i < static_cast<uint8>(ETutorialStage::TutorialEnd); ++i)
	{
		ETutorialStage Stage = static_cast<ETutorialStage>(i);

		
		if (UC_TutorialGoalWidget* TutorialGoalWidget = Cast<UC_TutorialGoalWidget>(GetWidgetFromName(*TutorialGoalWidgetNames[Stage])))
			TutorialGoalWidgets.Add(Stage, TutorialGoalWidget);
		else UC_Util::Print("From UC_TutorialWidget::NativeConstruct : TutorialSideGoalWidget casting failed!", FColor::Red, 10.f);

		if (UC_TutorialGoalExplanationContainer* GoalExplanation =
			Cast<UC_TutorialGoalExplanationContainer>(GetWidgetFromName(*TutorialGoalExplanationNames[Stage])))
		{
			TutorialGoalExplanations.Add(Stage, GoalExplanation);
		}
		else UC_Util::Print("From UC_TutorialWidget::NativeConstruct : TutorialGoalExplanation casting failed!", FColor::Red, 10.f);
	}

	SetIsFocusable(true);

	StageExplanationPanel->SetRenderOpacity(0.f);
}

void UC_TutorialWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	float NewOpacity = FMath::Lerp(StageExplanationPanel->GetRenderOpacity(), StageExplanationPanelOpacityDest, InDeltaTime * 10.f);
	StageExplanationPanel->SetRenderOpacity(NewOpacity);

	if (NewOpacity > 0.5f)
	{
		UpdateSpaceBarPercent();
		
		if (bSpaceBarDown)
		{
			float NewPercent = FMath::Min(SpaceBarPercent + InDeltaTime * 1.f, 1.f);
			SetSpaceBarProgressBarPercent(NewPercent);

			if (NewPercent >= 1.f) // Stage Start
			{
				bSpaceBarDown = false;
				ToggleStageExplanationPanel(false);
				
				// 현 Stage Delegate 초기화 & 기본 Start setting
				TutorialManager->InitCurrentStageChecker();
			}
		}
	}
}

void UC_TutorialWidget::SetStageExplanationPanel(ETutorialStage TutorialStage)
{
	// Set Title text
	StageTitle->SetText(FText::FromString(TutorialStageTitles[TutorialStage]));

	// 영상 setting
	if (!MediaPlayer->OpenSource(MediaSources[TutorialStage]))
		UC_Util::Print("MediaPlayer Opensource failed!", FColor::Red, 10.f);
	
	// Explanation 내용 setting
	for (TTuple<ETutorialStage, UCanvasPanel*> Tuple : StageExplanations)
		Tuple.Value->SetVisibility((TutorialStage == Tuple.Key) ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
}

void UC_TutorialWidget::ToggleStageExplanationPanel(bool InIsEnabled)
{
	AC_PlayerController* PC = Cast<AC_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	
	if (InIsEnabled)
	{
		StageExplanationPanelOpacityDest = 1.f;
		
		PC->SetInputMode(FInputModeUIOnly().SetWidgetToFocus(this->TakeWidget()));
		PC->SetIgnoreLookInput(true);
		PC->SetIgnoreMoveInput(true);

		// 움직임은 멈추는데 이상하게 Move함수 호출은 직전에 움직였다면 계속 호출됨 / 동작 Animation을 멈추기 위한 처리
		GAMESCENE_MANAGER->GetPlayer()->SetNextSpeed(0.f);
		GAMESCENE_MANAGER->GetPlayer()->SetCanMove(false);
		
		if (!MediaPlayer->Rewind()) UC_Util::Print("Rewind failed!", FColor::Red, 10.f);
		if (!MediaPlayer->Play())   UC_Util::Print("Play failed!", FColor::Red, 10.f);

		SetSpaceBarProgressBarPercent(0.f);
		TutorialVideoImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		StageExplanationPanelOpacityDest = 0.f;

		PC->SetInputMode(FInputModeGameOnly());
		PC->SetIgnoreLookInput(false);
		PC->SetIgnoreMoveInput(false);

		GAMESCENE_MANAGER->GetPlayer()->SetCanMove(true);
		TutorialVideoImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

FReply UC_TutorialWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() != EKeys::SpaceBar || InKeyEvent.IsRepeat()) return Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	// 키가 처음 눌리기 시작했을 때
	bSpaceBarDown = true;
	
	return FReply::Handled();
}

FReply UC_TutorialWidget::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() != EKeys::SpaceBar) return Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	bSpaceBarDown = false;
	SetSpaceBarProgressBarPercent(0.f);
	
	return FReply::Handled();
}

UC_TutorialGoalWidget* UC_TutorialWidget::GetCurrentTutorialGoalWidget() const
{
	if (!TutorialGoalWidgets.Contains(TutorialManager->GetCurrentStage())) return nullptr;
	return TutorialGoalWidgets[TutorialManager->GetCurrentStage()];
}

UC_TutorialGoalWidget* UC_TutorialWidget::GetTutorialGoalWidget(ETutorialStage TutorialStage) const
{
	if (!TutorialGoalWidgets.Contains(TutorialStage)) return nullptr;
	return TutorialGoalWidgets[TutorialStage];	
}

UC_TutorialGoalExplanationContainer* UC_TutorialWidget::GetCurrentStageGoalExplanationContainer() const
{
	if (!TutorialGoalExplanations.Contains(TutorialManager->GetCurrentStage())) return nullptr;
	return TutorialGoalExplanations[TutorialManager->GetCurrentStage()];
}

bool UC_TutorialWidget::StartTutorialGoalExplanation(ETutorialStage TargetTutorialStage, uint8 TargetGoalIndex)
{
	if (!TutorialGoalExplanations.Contains(TargetTutorialStage)) return false;
	return TutorialGoalExplanations[TargetTutorialStage]->StartTargetGoalExplanation(TargetGoalIndex);
}

bool UC_TutorialWidget::StopTutorialGoalExplanation(ETutorialStage TargetTutorialStage)
{
	if (!TutorialGoalExplanations.Contains(TargetTutorialStage)) return false;
	TutorialGoalExplanations[TargetTutorialStage]->StopExplanations();
	return true;
}
