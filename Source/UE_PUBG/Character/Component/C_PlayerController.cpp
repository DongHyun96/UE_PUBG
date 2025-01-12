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

	//// Input Mode�� GameAndUI�� ����
	//FInputModeGameAndUI InputMode;
	//InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // ���콺�� ����Ʈ�� �������� ����
	//InputMode.SetHideCursorDuringCapture(false); // ĸó �� Ŀ���� ������ ����
	//SetInputMode(InputMode);
	//
	//// ���콺 Ŀ�� ǥ��
	//bShowMouseCursor = true;
}

//void AC_PlayerController::SetupInputComponent()
//{
//	Super::SetupInputComponent();
//
//	// ���콺 ȸ�� �Է� ���ε�
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
//			ControlledPawn->AddControllerYawInput(Value); // Yaw ȸ��
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
//			ControlledPawn->AddControllerPitchInput(Value); // Pitch ȸ��
//		}
//	}
//}
