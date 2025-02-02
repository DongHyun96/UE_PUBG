// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_InvenSystem.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_PlayerController.h"
#include "Character/C_BasicCharacter.h"

#include "Item/Weapon/Gun/C_Gun.h"

#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/GameplayStatics.h"

#include "Utility/C_Util.h"

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

	PlayerController = Cast<AC_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	//PlayerController = Cast<AC_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController)
		PlayerController->SetIgnoreLookInput(false);

	if (!InvenUI && InvenUiClass)
	{
		//InvenUI = CreateWidget<UC_InvenUiWidget>(PlayerController, InvenUiClass);
		InvenUI = CreateWidget<UC_InventoryUIWidget>(PlayerController, InvenUiClass);
		if (InvenUI)
		{
			//InvenUI->SetOwnerCharacter(OwnerCharacter);
			InvenUI->SetOwnerPlayer(Cast<AC_Player>(OwnerCharacter));
			
			//InvenUI->SetWidgetsOwner(OwnerCharacter);
			if (!InvenUI->IsInViewport())
			{
				//InvenUI->AddToViewport();

				UC_Util::Print("Adding to viewport", FColor::Red, 10.f);
			}
			InvenUI->UpdateWidget();
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
		//PlayerController = GetWorld()->GetFirstPlayerController();
		//InvenUI = CreateWidget<UC_InvenUiWidget>(PlayerController, InvenUiClass);
		//InvenUI->SetOwnerCharacter(OwnerCharacter);

		InvenUI = CreateWidget<UC_InventoryUIWidget>(PlayerController, InvenUiClass);
		InvenUI->SetOwnerPlayer(Cast<AC_Player>(OwnerCharacter));

		if (!InvenUI->IsInViewport())
		{
			//InvenUI->AddToViewport();

			UC_Util::Print("Adding to viewport", FColor::Red, 10.f);
		}

		//InvenUI->SetVisibility(ESlateVisibility::Hidden);

	}
	
	//InvenUI->InitListView();
	//InvenUI->InitWidget();
	InvenUI->UpdateWidget();
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

	if (InvenUI->GetIsPanelOpened())
	{
		CloseInvenUI();
	}
	else
	{
		//InitializeList();
		ShowInvenUI();


	}

	if (!IsValid(OwnerCharacter)) return;

	// TODO : È®ÀÎ
	UCharacterMovementComponent* CharacterMovement = OwnerCharacter->GetCharacterMovement();
	if (CharacterMovement)
		CharacterMovement->SetMovementMode(EMovementMode::MOVE_Walking);

}

void UC_InvenSystem::ShowInvenUI()
{
	if (!InvenUI)
	{
		UC_Util::Print("From UC_InvenSystem::ShowInvenUI : InvenUI Nullptr", FColor::Red, 10.f);
		return;
	}
	//PlayerController = GetWorld()->GetFirstPlayerController();
	InvenUI->SetVisibility(ESlateVisibility::Visible);
}

void UC_InvenSystem::CloseInvenUI()
{
	if (!InvenUI)
	{  
		UC_Util::Print("From UC_InvenSystem::CloseInvenUI : InvenUI Nullptr", FColor::Red, 10.f);
		return;
	}
	//PlayerController = GetWorld()->GetFirstPlayerController();
	InvenUI->SetVisibility(ESlateVisibility::Hidden);
}


// Called every frame
void UC_InvenSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

