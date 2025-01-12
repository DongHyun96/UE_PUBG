// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_PlayerController.h"
#include "Framework/Application/NavigationConfig.h"
void AC_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		TSharedRef<FNavigationConfig> Navigation = MakeShared<FNavigationConfig>();
		Navigation->bKeyNavigation = false;
		Navigation->bTabNavigation = false;
		Navigation->bAnalogNavigation = false;
		FSlateApplication::Get().SetNavigationConfig(Navigation);
	}

	//// Input Mode를 GameAndUI로 설정
	//FInputModeGameAndUI InputMode;
	//InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // 마우스가 뷰포트에 고정되지 않음
	//InputMode.SetHideCursorDuringCapture(false); // 캡처 중 커서를 숨기지 않음
	//SetInputMode(InputMode);
	//
	//// 마우스 커서 표시
	//bShowMouseCursor = true;
}

//void AC_PlayerController::SetupInputComponent()
//{
//	Super::SetupInputComponent();
//
//	// 마우스 회전 입력 바인딩
//	InputComponent->BindAxis("Turn", this, &AC_PlayerController::HandleTurn);
//	InputComponent->BindAxis("LookUp", this, &AC_PlayerController::HandleLookUp);
//}
//
//void AC_PlayerController::HandleTurn(float Value)
//{
//	if (Value != 0.0f)
//	{
//		APawn* ControlledPawn = GetPawn();
//		if (ControlledPawn)
//		{
//			ControlledPawn->AddControllerYawInput(Value); // Yaw 회전
//		}
//	}
//}
//
//void AC_PlayerController::HandleLookUp(float Value)
//{
//	if (Value != 0.0f)
//	{
//		APawn* ControlledPawn = GetPawn();
//		if (ControlledPawn)
//		{
//			ControlledPawn->AddControllerPitchInput(Value); // Pitch 회전
//		}
//	}
//}
