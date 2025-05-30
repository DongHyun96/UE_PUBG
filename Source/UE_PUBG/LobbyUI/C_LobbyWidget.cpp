// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyUI/C_LobbyWidget.h"

#include "C_ToggleButtonGroupWidget.h"
#include "C_ToggleButtonWidget.h"
#include "Character/Component/C_PlayerController.h"
#include "Components/BackgroundBlur.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "LobbyPlayerPawn/C_LobbyPawn.h"
#include "Singleton/C_GameInstance.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"

void UC_LobbyWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	MapSelectToggleButtonGroup->Init(this);
	
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

	if (LoginConfirmButton) LoginConfirmButton->OnReleased.AddDynamic(this, &UC_LobbyWidget::OnLogInConfirmButtonReleased);

	if (NickNameTextBox)
	{
		NickNameTextBox->OnTextChanged.AddDynamic(this, &UC_LobbyWidget::OnNickNameTextBoxTextChanged);
		NickNameTextBox->OnTextCommitted.AddDynamic(this, &UC_LobbyWidget::OnNickNameTextBoxCommitted);
	}
	
	InitMainLobby();

	if (AC_PlayerController* PlayerController = Cast<AC_PlayerController>(GetOwningPlayer()))
		PlayerController->SetLobbyWidget(this);
	else UC_Util::Print("From UC_LobbyWidget::NativeConstruct : Failed to get PlayerController", FColor::Red, 10.f);

	// TODO : 현재 최초 로그인인지 확인하여, 맞다면 Login 처리 먼저 진행
	if (UC_GameInstance* GameInstance = Cast<UC_GameInstance>(GetGameInstance()))
		CurrentLobbyPageLocation = GameInstance->GetPlayerNickNameSet() ? ELobbyPageLocation::MainLobby : ELobbyPageLocation::LogIn;

	// Init each widget positions by First Lobby page location (Pawn Initial 위치도 어쩔 수 없이 여기서 정해주어야 함)
	// -> 아니면 GameInstance 직접 확인함으로써 적용하기(이게 더 괜찮을듯?)
	if (CurrentLobbyPageLocation == ELobbyPageLocation::LogIn) // First page - Log in page
	{
		bMainLobbyUIHidden = true;
		HideMainLobbyUIOnWidgetStart();
		FTimerHandle& TimerHandle = GAMESCENE_MANAGER->GetTimerHandle();
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			PlayAnimation(LoginPanelSlideIn);
			NickNameTextBox->SetFocus();
			BlurLerpDest = 10.f;
		}, 1.5f, false);
	}
}

void UC_LobbyWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	float BackgroundBlurStrength = FMath::Lerp(BackgroundBlur->GetBlurStrength(), BlurLerpDest, 5.f * InDeltaTime);
	BackgroundBlur->SetBlurStrength(BackgroundBlurStrength);
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

void UC_LobbyWidget::OnNickNameTextBoxTextChanged(const FText& Text)
{
	// 제한 사항 적용 : 공백x & 14자 제한 & 영문입력만 받기
	FString ReceivedString = Text.ToString();

	if (ReceivedString.IsEmpty()) return;
	TCHAR LastChar = ReceivedString[ReceivedString.Len() - 1];
	
	if (LastChar == ' ')
	{
		WarningText->SetText(FText::FromString("No spaces allowed"));
		PlayAnimation(NickNameCharacterWarningAnimation);
		ReceivedString.RemoveAt(ReceivedString.Len() - 1);
	}
	else if (!((LastChar >= 'A' && LastChar <= 'Z') || (LastChar >= 'a' && LastChar <= 'z'))) // 알파벳 테스팅
	{
		WarningText->SetText(FText::FromString("Only alphabet letters are allowed"));
		PlayAnimation(NickNameCharacterWarningAnimation);
		ReceivedString.RemoveAt(ReceivedString.Len() - 1);
	}
	
	// ReceivedString = ReceivedString.Replace(TEXT(" "), TEXT(""));

	if (ReceivedString.Len() > 14)
	{
		WarningText->SetText(FText::FromString("Maximum of 14 characters allowed"));
		PlayAnimation(NickNameCharacterWarningAnimation);
		ReceivedString = ReceivedString.LeftChop(1);
		// ReceivedString.RemoveAt(ReceivedString.Len() - 1);
	}
	
	NickNameTextBox->SetText(FText::FromString(ReceivedString));
}

void UC_LobbyWidget::OnNickNameTextBoxCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod != ETextCommit::Type::OnEnter) return;
	OnLogInConfirmButtonReleased();	
}

void UC_LobbyWidget::OnLogInConfirmButtonReleased()
{
	if (CurrentLobbyPageLocation != ELobbyPageLocation::LogIn) return;
	FString ReceivedName = NickNameTextBox->GetText().ToString();
	if (ReceivedName.IsEmpty())
	{
		WarningText->SetText(FText::FromString("Nick name cannot be empty"));
		PlayAnimation(NickNameCharacterWarningAnimation);
		return;
	}
	
	AC_PlayerController* PlayerController = Cast<AC_PlayerController>(GetWorld()->GetFirstPlayerController()); 

	if (!PlayerController)
	{
		UC_Util::Print("From UC_LobbyWidget::OnLogInConfirmButtonReleased : Cannot find PlayerController!", FColor::Red, 10.f);
		return;
	}
	
	// EditableTextBlock 입력 후, 화면을 한 번 클릭하고 나서야 다른 키보드 Input 처리가 가능했음
	// 바로 다른 키보드 Input 처리(IMC_Lobby) 가능하게끔 setting
	PlayerController->SetInputMode
	(
		FInputModeGameAndUI()
		.SetWidgetToFocus(this->TakeWidget())
		.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock)
		.SetHideCursorDuringCapture(false)
	);

	PlayerController->bShowMouseCursor = true;
	PlayerController->SetIgnoreLookInput(true);
	
	FSlateApplication::Get().ClearKeyboardFocus(EFocusCause::Cleared);

	BlurLerpDest = 0.f;
	
	UC_GameInstance* GameInstance = Cast<UC_GameInstance>(GetGameInstance());
	GameInstance->SetPlayerNickName(ReceivedName);

	AC_LobbyPawn* LobbyPawn = Cast<AC_LobbyPawn>(PlayerController->GetPawn());
	LobbyPawn->SetLocationLerpDestinationToMainLobby();
	LobbyPawn->InitCharacterNameTag();
	
	PlayAnimation(LoginPanelSlideOut);

	// Main Lobby 전환 setting
	FTimerHandle& TimerHandle = GAMESCENE_MANAGER->GetTimerHandle();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		CurrentLobbyPageLocation = ELobbyPageLocation::MainLobby;
		OnLobbySpaceBarDown();
	}, 2.f, false);
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
	if (!SettingButton->IsHovered()) return;

	AC_PlayerController* PlayerController = Cast<AC_PlayerController>(GetWorld()->GetFirstPlayerController()); 

	if (!PlayerController)
	{
		UC_Util::Print("From UC_LobbyWidget::OnSettingButtonReleased : Cannot find PlayerController!", FColor::Red, 10.f);
		return;
	}

	PlayerController->ToggleMainMenu();
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
