// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_PlayerController.h"
#include "Framework/Application/NavigationConfig.h"
#include "Singleton/C_GameSceneManager.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
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
}

void AC_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	/// Script / EnhancedInput.InputAction'/Game/Project_PUBG/Common/Character/Input/IA_MainMenu.IA_MainMenu'


	InputMappingContext = LoadObject<UInputMappingContext>(
		nullptr, TEXT("/Game/Project_PUBG/Common/Character/Input/IMC_PlayerController.IMC_PlayerController"));
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (InputMappingContext)
			Subsystem->AddMappingContext(InputMappingContext, 0); // Priority 0
		else
		{
			UC_Util::Print("InputMappingContext is nullptr", FColor::Red, 10.f);
		}
	}

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

}

void AC_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInput->BindAction(OpenMenuAction, ETriggerEvent::Started, this, &AC_PlayerController::ToggleMainMenu);
	}
}

void AC_PlayerController::ToggleMainMenu()
{
	UC_Util::Print("Down ToggleMainMenu");

	if (!InputMappingContext)
	{
		UC_Util::Print("InputMappingContext is nullptr", FColor::Red, 10.f);
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
		UC_Util::Print("MainMenuWidget is nullptr", FColor::Red, 10.f);
		return;
	}

	// TODO : Collapsed 는 메인메뉴창을 종료한 상태,
	//      : Hidden은 메인메뉴창에서 Preperences같은 다른 창을 띄운 상태
	if (MainMenuWidget->GetVisibility() == ESlateVisibility::Hidden || MainMenuWidget->GetVisibility() == ESlateVisibility::Collapsed)
	{
		MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
		SetPause(true);
		bShowMouseCursor = true;
		UC_Util::Print("MainMenuWidget is added to viewport", FColor::Green, 10.f);

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