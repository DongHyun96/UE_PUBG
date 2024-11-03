// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_InvenSystem.h"
#include "Character/C_BasicCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UC_InvenSystem::UC_InvenSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UC_InvenSystem::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = GetWorld()->GetFirstPlayerController();
	InvenUI = CreateWidget<UC_InvenUiWidget>(PlayerController, InvenUiClass);
	InvenUI->SetOwnerCharacter(OwnerCharacter);

	
}

void UC_InvenSystem::InitializeList()
{
	if (!IsValid(InvenUI)) return;

	InvenUI->InitListView();
}

void UC_InvenSystem::OpenInvenUI()
{
	//if (!InvenUI) return;	
	if (!IsValid(InvenUI)) return;

	if (isPanelOpened)
	{
		//UI�� ���� �ִٸ�.
		isPanelOpened = false;
		InvenUI->SetVisibility(ESlateVisibility::Hidden);
		InvenUI->RemoveFromViewport();
		//InvenUI = nullptr;

		PlayerController->SetIgnoreLookInput(false);

		FInputModeGameOnly InputMode;

		//PlayerController->SetInputMode(InputMode);

		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->bShowMouseCursor = false;


	}
	else
	{
		//UI�� ���� �ִٸ�.
		isPanelOpened = true;

		InvenUI->InitWidget(); //�Ʒ����� �ߺ�ó�� �ȴٸ� ���ֱ�.
		InvenUI->SetVisibility(ESlateVisibility::Visible);
		InvenUI->AddToViewport();

		PlayerController->SetIgnoreLookInput(true);

		FInputModeGameAndUI InputMode;
		//InputMode.SetWidgetToFocus(InvenUI->TakeWidget());
		InputMode.SetWidgetToFocus(nullptr);

		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(true);

		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = true;
	}

	UCharacterMovementComponent* CharacterMovement = OwnerCharacter->GetCharacterMovement();
	if (CharacterMovement)
		CharacterMovement->SetMovementMode(EMovementMode::MOVE_Walking);

}


// Called every frame
void UC_InvenSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

