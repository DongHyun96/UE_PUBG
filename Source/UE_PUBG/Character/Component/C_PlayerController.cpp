// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_PlayerController.h"
#include "Framework/Application/NavigationConfig.h"
#include "Singleton/C_GameSceneManager.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Character/C_Player.h"
#include "LobbyUI/C_LobbyWidget.h"
#include "Singleton/C_GameInstance.h"
#include "Utility/C_Util.h"
AC_PlayerController::AC_PlayerController()
{
	/// Script / UMGEditor.WidgetBlueprint'/Game/Project_PUBG/Common/UI/test/MainMenu/WBP_Menu.WBP_Menu'
	// MainMenu 위젯을 생성하기 위한 클래스 로드, WBP_Menu의 경로나 이름이 변경되면 수정 필요
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuWidgetBPClass(TEXT("/Game/Project_PUBG/Common/UI/MainMenu/WBP_Menu"));
	//static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuWidgetBPClass(TEXT("/Game/Project_PUBG/Common/UI/test/MainMenu/WBP_Menu"));

	if (MainMenuWidgetBPClass.Class != nullptr)
	{
		MainMenuClass = MainMenuWidgetBPClass.Class;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_OpenMenu(TEXT("/Game/Project_PUBG/Common/Character/Input/IA_MainMenu"));
	if (IA_OpenMenu.Succeeded())
	{
		OpenMenuAction = IA_OpenMenu.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_LobbyFKey(TEXT("/Game/Project_PUBG/DongHyun/LobbyMap/LobbyInput/IA_LobbyF"));
	if (IA_LobbyFKey.Succeeded()) IALobbyFKeyAction = IA_LobbyFKey.Object;
		
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_LobbySpaceBar(TEXT("/Game/Project_PUBG/DongHyun/LobbyMap/LobbyInput/IA_LobbySpaceBar"));
	if (IA_LobbySpaceBar.Succeeded()) IALobbySpaceBarAction = IA_LobbySpaceBar.Object;
}

void AC_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	/// Script / EnhancedInput.InputAction'/Game/Project_PUBG/Common/Character/Input/IA_MainMenu.IA_MainMenu'

	InputMappingContext = LoadObject<UInputMappingContext>(
		nullptr, TEXT("/Game/Project_PUBG/Common/Character/Input/IMC_PlayerController.IMC_PlayerController"));
	
	if (!AddIMCToSubsystem(InputMappingContext, 0))
		UC_Util::Print("Adding IMC_PlayerController failed!", FColor::Red, 10.f);

	if (IsLocalPlayerController())
	{
		TSharedRef<FNavigationConfig> Navigation = MakeShared<FNavigationConfig>();
		Navigation->bKeyNavigation = false;
		Navigation->bTabNavigation = false;
		Navigation->bAnalogNavigation = false;
		FSlateApplication::Get().SetNavigationConfig(Navigation);
	}

	// MainMenu 위젯이 에디터에서 지정되었으면, 생성
	if (MainMenuClass)
	{
		MainMenuWidget = CreateWidget<UUserWidget>(this, MainMenuClass);
		MainMenuWidget->AddToViewport(40);
		MainMenuWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	//SetupInputComponent();

	// 만일 이 Controller를 사용중인 Pawn이 AC_Player이라면(In Game Level) -> 반대로 아니라면, 현재 Lobby Level
	if (!Cast<AC_Player>(GetPawn()))
	{
		// Lobby Level일 때의 Input Mapping Context 더 추가해주기
		UInputMappingContext* LobbyIMC = LoadObject<UInputMappingContext>(
		nullptr, TEXT("/Game/Project_PUBG/Common/Character/Input/IMC_Lobby.IMC_Lobby"));

		if (!AddIMCToSubsystem(LobbyIMC, 1))
			UC_Util::Print("Adding Lobby IMC failed!", FColor::Red, 10.f);
	}
}

void AC_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInput->BindAction(OpenMenuAction, ETriggerEvent::Started, this, &AC_PlayerController::ToggleMainMenu);
		EnhancedInput->BindAction(IALobbyFKeyAction, ETriggerEvent::Completed, this, &AC_PlayerController::OnLobbyFKeyReleased);
		EnhancedInput->BindAction(IALobbySpaceBarAction, ETriggerEvent::Started, this, &AC_PlayerController::OnLobbySpaceBarDown);
	}
}

void AC_PlayerController::ToggleMainMenu()
{

	if (!InputMappingContext)
	{
		UC_Util::Print("From AC_PlayerController::ToggleMainMenu : InputMappingContext is nullptr", FColor::Red, 10.f);
		return;
	}
	//// 이미 열려 있으면 아무것도 하지 않고 리턴
	//if (MainMenuWidget && MainMenuWidget->IsInViewport())
	//{
	//	UC_Util::Print("MainMenuWidget is already in viewport", FColor::Red, 10.f);
	//	return;
	//}

	if (!MainMenuWidget)
	{
		UC_Util::Print("From AC_PlayerController::ToggleMainMenu : MainMenuWidget is nullptr", FColor::Red, 10.f);
		return;
	}
	
	if (!Cast<AC_Player>(GetPawn())) // Lobby map에서의 예외처리
	{
		if (IsValid(LobbyWidget) && LobbyWidget->GetCurrentLobbyPageLocation() == ELobbyPageLocation::LogIn) return;
	}

	// TODO : Collapsed 는 메인메뉴창을 종료한 상태,
	//      : Hidden은 메인메뉴창에서 Preferences같은 다른 창을 띄운 상태
	if (MainMenuWidget->GetVisibility() == ESlateVisibility::Hidden || MainMenuWidget->GetVisibility() == ESlateVisibility::Collapsed)
	{
		MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
		SetPause(true);
		bShowMouseCursor = true;

		// UIOnly 모드로 전환하고 위젯에 포커스
		FInputModeUIOnly InputModeData;
		InputModeData.SetWidgetToFocus(MainMenuWidget->TakeWidget());
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputModeData);
	}
	else if (MainMenuWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		//MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		//SetPause(true);
		//
		//GAMESCENE_MANAGER->SetCurrentHUDMode(EHUDMode::IDLE);
		//UC_Util::Print("MainMenuWidget is nullptr", FColor::Red, 10.f);
	}
	
}

bool AC_PlayerController::AddIMCToSubsystem(UInputMappingContext* IMC, int Priority)
{
	if (!IsValid(IMC))
	{
		UC_Util::Print("From AC_PlayerController::AddIMCToSubsystem : Invalid IMC received!", FColor::Red, 10.f);
		return false;
	}
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (!Subsystem)
	{
		UC_Util::Print("From AC_PlayerController::AddIMCToSubsystem : Subsystem not found", FColor::Red, 10.f);
		return false;
	}

	Subsystem->AddMappingContext(IMC, Priority);
	return true;
}

bool AC_PlayerController::RemoveIMCFromSubsystem(UInputMappingContext* IMC)
{
	if (!IsValid(IMC))
	{
		UC_Util::Print("From AC_PlayerController::AddIMCToSubsystem : Invalid IMC received!", FColor::Red, 10.f);
		return false;
	}
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (!Subsystem)
	{
		UC_Util::Print("From AC_PlayerController::AddIMCToSubsystem : Subsystem not found", FColor::Red, 10.f);
		return false;
	}
	
	Subsystem->RemoveMappingContext(IMC);
	return true;
}

void AC_PlayerController::OnLobbySpaceBarDown()
{
	if (!IsValid(LobbyWidget))
	{
		UC_Util::Print("From AC_PlayerController::OnLobbySpaceBarDown : Invalid LobbyWidget", FColor::Red, 10.f);
		return;
	}

	// Menu가 열려있다면 input 무시
	if (MainMenuWidget->GetVisibility() == ESlateVisibility::Visible ||
		MainMenuWidget->GetVisibility() == ESlateVisibility::SelfHitTestInvisible) return;
	
	LobbyWidget->OnLobbySpaceBarDown();
}

void AC_PlayerController::OnLobbyFKeyReleased()
{
	if (!IsValid(LobbyWidget))
	{
		UC_Util::Print("From AC_PlayerController::OnLobbyFKeyReleased : Invalid LobbyWidget", FColor::Red, 10.f);
		return;
	}

	// Menu가 열려있다면 input 무시
	if (MainMenuWidget->GetVisibility() == ESlateVisibility::Visible ||
		MainMenuWidget->GetVisibility() == ESlateVisibility::SelfHitTestInvisible) return;

	LobbyWidget->OnLobbyFKeyReleased();
}
