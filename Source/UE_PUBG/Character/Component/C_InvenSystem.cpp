// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_InvenSystem.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/C_BasicCharacter.h"

#include "Item/Weapon/Gun/C_Gun.h"

#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UC_InvenSystem::UC_InvenSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//InvenUI->AddToViewport();
	//InvenUI->SetVisibility(ESlateVisibility::Hidden);
	// ...
}


// Called when the game starts
void UC_InvenSystem::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = GetWorld()->GetFirstPlayerController();
	if (!InvenUI && InvenUiClass)
	{
		InvenUI = CreateWidget<UC_InvenUiWidget>(PlayerController, InvenUiClass);
		if (InvenUI)
		{
			InvenUI->SetOwnerCharacter(OwnerCharacter);
			//InvenUI->SetWidgetsOwner(OwnerCharacter);
			//InvenUI->AddToViewport();
			//InvenUI->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create InvenUI widget."));
		}
	}

	
}

void UC_InvenSystem::InitializeList()
{
	//if (!IsValid(InvenUI)) return;
	if (!IsValid(InvenUI))
	{
		PlayerController = GetWorld()->GetFirstPlayerController();
		InvenUI = CreateWidget<UC_InvenUiWidget>(PlayerController, InvenUiClass);
		InvenUI->SetOwnerCharacter(OwnerCharacter);
	}
	InvenUI->InitListView();
}

void UC_InvenSystem::OpenInvenUI()
{

	if (!IsValid(InvenUI)) return;	
	//if (!IsValid(InvenUI))
	//{
	//	PlayerController = GetWorld()->GetFirstPlayerController();
	//	InvenUI = CreateWidget<UC_InvenUiWidget>(PlayerController, InvenUiClass);
	//	InvenUI->SetOwnerCharacter(OwnerCharacter);
	//}


	

	if (isPanelOpened)
	{
		//UI가 열려 있다면.
		//isPanelOpened = false;
		////InvenUI->SetVisibility(ESlateVisibility::Hidden);
		////InvenUI = nullptr;
		////if (!IsValid(PlayerController))
		////{
		////	PlayerController = GetWorld()->GetFirstPlayerController();
		////}
		//PlayerController->SetIgnoreLookInput(false);

		//FInputModeGameOnly InputMode;

		////PlayerController->SetInputMode(InputMode);

		//PlayerController->SetInputMode(FInputModeGameOnly());
		//PlayerController->bShowMouseCursor = false;
		//InvenUI->RemoveFromViewport();
		////자꾸 터져서 가비지 컬랙터 대응용으로 해봄.
		////InvenUI->RemoveFromRoot();
		CloseInvenUI();

	}
	else
	{
		//UI가 닫혀 있다면.
		//자꾸 터져서 가비지 컬렉터 대응용으로 해봄.
		//InvenUI->AddToRoot();
		//isPanelOpened = true;
		//InvenUI->AddToViewport();

		//InvenUI->InitWidget(); //아래에서 중복처리 된다면 없애기.
		////InvenUI->SetVisibility(ESlateVisibility::Visible);
		////if (!IsValid(PlayerController))
		////{
		////	PlayerController = GetWorld()->GetFirstPlayerController();
		////}
		//PlayerController->SetIgnoreLookInput(true);

		//FInputModeGameAndUI InputMode;
		////InputMode.SetWidgetToFocus(InvenUI->TakeWidget());
		//InputMode.SetWidgetToFocus(nullptr);

		//InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		//InputMode.SetHideCursorDuringCapture(true);

		//PlayerController->SetInputMode(InputMode);
		//PlayerController->bShowMouseCursor = true;

		//if (OwnerCharacter->GetEquippedComponent()->GetCurWeapon())
		//	Cast<AC_Gun>(OwnerCharacter->GetEquippedComponent()->GetCurWeapon())->SetIsAimPress(false);
		InitializeList();
		ShowInvenUI();
	}

	if (!IsValid(OwnerCharacter)) return;

	UCharacterMovementComponent* CharacterMovement = OwnerCharacter->GetCharacterMovement();
	if (CharacterMovement)
		CharacterMovement->SetMovementMode(EMovementMode::MOVE_Walking);

}

void UC_InvenSystem::ShowInvenUI()
{
	isPanelOpened = true;
	//InvenUI->AddToViewport();

	if (!InvenUI->IsInViewport())
	{
		InvenUI->AddToViewport();
	}

	if (!PlayerController)
		PlayerController = GetWorld()->GetFirstPlayerController();
	
	PlayerController->SetIgnoreLookInput(true);

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(InvenUI->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;

	InvenUI->SetVisibility(ESlateVisibility::Visible);
}

void UC_InvenSystem::CloseInvenUI()
{
	isPanelOpened = false;

	if (!PlayerController) 
		PlayerController = GetWorld()->GetFirstPlayerController();


	PlayerController->SetIgnoreLookInput(false);

	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);

	PlayerController->bShowMouseCursor = false;


	if (InvenUI)
	{
		//InvenUI->RemoveFromViewport();
		InvenUI->SetVisibility(ESlateVisibility::Hidden);
	}
}


// Called every frame
void UC_InvenSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

