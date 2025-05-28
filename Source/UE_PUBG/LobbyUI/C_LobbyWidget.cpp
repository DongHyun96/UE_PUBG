// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyUI/C_LobbyWidget.h"

#include "C_ToggleButtonGroupWidget.h"
#include "C_ToggleButtonWidget.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Character/Component/C_PlayerController.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Singleton/C_GameInstance.h"
#include "Utility/C_Util.h"

void UC_LobbyWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	MapSelectToggleButtonGroup->Init(this);

	// GetWidgetFromName()

	// Init MapSelectionBelowBars
	for (int i = 0; i < static_cast<uint8>(ELevelType::Max); ++i)
	{
		FString BarWidgetName = FString::Printf(TEXT("BarImage_%d"), i);
		FName WidgetName(*BarWidgetName);
		UImage* BarImage = Cast<UImage>(GetWidgetFromName(WidgetName));

		if (!BarImage)
		{
			UC_Util::Print("From UC_LobbyWidget::NativePreConstruct : Cannot find Bar image!", FColor::Red, 10.f);
			continue;
		}
		MapSelectionBelowBars.Add(BarImage);
	}
}

void UC_LobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (SelectGameMapButton)
	{
		// Standalone에서 OnClicked call back을 받을 수 없어서(이유는 잘 모르겠음), Released로 걸어둠
		// SelectGameMapButton->OnClicked.AddDynamic(this, &UC_LobbyWidget::OnSelectGameMapButtonClicked);
		SelectGameMapButton->OnReleased.AddDynamic(this, &UC_LobbyWidget::OnSelectGameMapButtonReleased);
		SelectGameMapButton->OnHovered.AddDynamic(this, &UC_LobbyWidget::OnSelectGameMapButtonHovered);	
		SelectGameMapButton->OnUnhovered.AddDynamic(this, &UC_LobbyWidget::OnSelectGameMapButtonUnhovered);
	}

	if (SettingButton)
	{
		SettingButton->OnReleased.AddDynamic(this, &UC_LobbyWidget::OnSettingButtonReleased);
		SettingButton->OnHovered.AddDynamic(this, &UC_LobbyWidget::OnSettingButtonHovered);	
		SettingButton->OnUnhovered.AddDynamic(this, &UC_LobbyWidget::OnSettingButtonUnhovered);
	}

	if (ConfirmButton) ConfirmButton->OnReleased.AddDynamic(this, &UC_LobbyWidget::OnMapSelectionPanelConfirmButtonReleased);
	if (CancelButton) CancelButton->OnReleased.AddDynamic(this, &UC_LobbyWidget::OnMapSelectionPanelCancelButtonReleased);

	if (LeftButton) LeftButton->OnReleased.AddDynamic(this, &UC_LobbyWidget::OnLeftButtonReleased);
	if (RightButton) RightButton->OnReleased.AddDynamic(this, &UC_LobbyWidget::OnRightButtonReleased);

	if (PlayButton) PlayButton->OnReleased.AddDynamic(this, &UC_LobbyWidget::OnPlayButtonReleased);

	InitMainLobby();

	if (AC_PlayerController* PlayerController = Cast<AC_PlayerController>(GetOwningPlayer()))
		PlayerController->SetLobbyWidget(this);
	else UC_Util::Print("From UC_LobbyWidget::NativeConstruct : Failed to get PlayerController", FColor::Red, 10.f);
}

/*void UC_LobbyWidget::SetCurrentLobbyPageLocation(ELobbyPageLocation InLobbyPageLocation)
{
	ELobbyPageLocation PrevLobbyPageLocation = CurrentLobbyPageLocation;
	CurrentLobbyPageLocation = InLobbyPageLocation;
}*/

void UC_LobbyWidget::OnSelectedMapChanged(ELevelType InSelected)
{
	PlayAnimation(BackgroundMapImageAnimation);
	PlayAnimation(MapNameSizeAnimation);
	MapSelectionMapImage->SetBrushFromTexture(LevelSelectionBackGroundImages[InSelected]);
	
	switch (InSelected)
	{
	case ELevelType::QuickStart:
		MapSelectedNameText->SetText(FText::FromString("QUICK START"));
		MapSelectedSizeText->SetText(FText::FromString("?X? MAP"));
		MapSelectedSizeText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	case ELevelType::ShantyTown:
		MapSelectedNameText->SetText(FText::FromString("SHANTY TOWN"));
		MapSelectedSizeText->SetText(FText::FromString("1X1 MAP"));
		MapSelectedSizeText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	case ELevelType::TrainingGround:
		MapSelectedNameText->SetText(FText::FromString("TRAINING GROUND"));
		MapSelectedSizeText->SetVisibility(ESlateVisibility::Hidden);
		break;
	}
}

void UC_LobbyWidget::OnLobbySpaceBarDown()
{
	if (CurrentLobbyPageLocation != ELobbyPageLocation::MainLobby) return;

	PlayAnimation(bMainLobbyUIHidden ? ShowLobbyUI : HideLobbyUI);
	
	bMainLobbyUIHidden = !bMainLobbyUIHidden;
}

void UC_LobbyWidget::OnLobbyFKeyPressed()
{
	if (CurrentLobbyPageLocation != ELobbyPageLocation::MainLobby) return;
}

void UC_LobbyWidget::OnLobbyFKeyReleased()
{
	if (CurrentLobbyPageLocation != ELobbyPageLocation::MainLobby) return;
	OnPlay();
}

TSoftObjectPtr<UWorld> UC_LobbyWidget::GetRandomLevel()
{
	uint8 RandomLevelNum = FMath::RandRange(static_cast<uint8>(1), static_cast<uint8>(ELevelType::Max) - 1);
	return Levels[static_cast<ELevelType>(RandomLevelNum)];
}

void UC_LobbyWidget::InitMainLobby()
{
	UC_GameInstance* GameInstance = Cast<UC_GameInstance>(GetGameInstance());
	if (!GameInstance) return;
	ELevelType SelectedLevelType = GameInstance->GetCurrentSelectedLevelType();

	SelectGameMapImage->SetBrushFromTexture(SelectGameMapImageTextures[SelectedLevelType]);

	FString MapName = SelectedLevelType == ELevelType::QuickStart ? "QUICK START" :
					  SelectedLevelType == ELevelType::ShantyTown ? "SHANTY TOWN" : "TRAINING GROUND";

	SelectedMapName->SetText(FText::FromString(MapName));
	UpdateSelectedMapBars(SelectedLevelType);
}

void UC_LobbyWidget::OnPlayButtonReleased()
{
	if (!PlayButton->IsHovered()) return;
	OnPlay();
}

void UC_LobbyWidget::OnLeftButtonReleased()
{
	if (!LeftButton->IsHovered()) return;
	
	UC_GameInstance* GameInstance = Cast<UC_GameInstance>(GetGameInstance());
	if (!GameInstance)
	{
		UC_Util::Print("From UC_LobbyWidget::OnLeftButtonReleased : GameInstance casting failed!", FColor::Red, 10.f);
		return;
	}

	GameInstance->DecreaseCurrentSelectedLevelType();
	InitMainLobby();
}

void UC_LobbyWidget::OnRightButtonReleased()
{
	if (!RightButton->IsHovered()) return;
	
	UC_GameInstance* GameInstance = Cast<UC_GameInstance>(GetGameInstance());
	if (!GameInstance)
	{
		UC_Util::Print("From UC_LobbyWidget::OnLeftButtonReleased : GameInstance casting failed!", FColor::Red, 10.f);
		return;
	}

	GameInstance->IncreaseCurrentSelectedLevelType();
	InitMainLobby();
}

void UC_LobbyWidget::UpdateSelectedMapBars(ELevelType InSelectedMapType)
{
	for (UImage* BarImage : MapSelectionBelowBars)
		BarImage->SetColorAndOpacity(FLinearColor(0.1f, 0.1f, 0.1f, 1.f));

	int SelectedIndex = static_cast<int>(InSelectedMapType);
	MapSelectionBelowBars[SelectedIndex]->SetColorAndOpacity(FLinearColor(0.8f, 0.8f, 0.8f, 1.f));
}

void UC_LobbyWidget::OnSelectGameMapButtonReleased()
{
	if (!SelectGameMapButton->IsHovered()) return;
	MapSelectionPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	MapSelectToggleButtonGroup->Init();
	CurrentLobbyPageLocation = ELobbyPageLocation::MapSelectionPanel;
	ToggleLobbyUIText->SetVisibility(ESlateVisibility::Hidden);
}

void UC_LobbyWidget::OnSelectGameMapButtonHovered()
{
	SelectGameMapHoveredPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UC_LobbyWidget::OnSelectGameMapButtonUnhovered()
{
	SelectGameMapHoveredPanel->SetVisibility(ESlateVisibility::Hidden);
}

void UC_LobbyWidget::OnSettingButtonReleased()
{
	if (SettingButton->IsHovered()) return;
	
	// TODO : 설정창 열기
}

void UC_LobbyWidget::OnSettingButtonHovered()
{
	SettingButtonImageHoverBG->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UC_LobbyWidget::OnSettingButtonUnhovered()
{
	SettingButtonImageHoverBG->SetVisibility(ESlateVisibility::Hidden);
}

void UC_LobbyWidget::OnMapSelectionPanelConfirmButtonReleased()
{
	if (!ConfirmButton->IsHovered()) return;
	MapSelectionPanel->SetVisibility(ESlateVisibility::Hidden);
	
	UC_GameInstance* GameInstance = Cast<UC_GameInstance>(GetGameInstance());
	if (!GameInstance) return;

	GameInstance->SetCurrentSelectedLevelType(MapSelectToggleButtonGroup->GetCurrentSelectedButton()->GetLevelType());
	CurrentLobbyPageLocation = ELobbyPageLocation::MainLobby;
	ToggleLobbyUIText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	InitMainLobby();
}

void UC_LobbyWidget::OnMapSelectionPanelCancelButtonReleased()
{
	if (!CancelButton->IsHovered()) return;
	MapSelectionPanel->SetVisibility(ESlateVisibility::Hidden);
	CurrentLobbyPageLocation = ELobbyPageLocation::MainLobby;
	ToggleLobbyUIText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	InitMainLobby();
}

FReply UC_LobbyWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	UC_Util::Print("asdf", FColor::Red, 10.f);

	// TODO : 입력 여기서 처리되었을 때 Handled 처리하기
	// return FReply::Handled();

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
