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

UC_InvenSystem::UC_InvenSystem()
{
	PrimaryComponentTick.bCanEverTick = true;


}

void UC_InvenSystem::BeginPlay()
{
	Super::BeginPlay();

	// TODO : GameSceneManager에서 PlayerController를 가져오는 방법으로 변경하는게 나은가?
	PlayerController = Cast<AC_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController)
		PlayerController->SetIgnoreLookInput(false);

	if (!InvenUI && InvenUiClass)
	{
		InvenUI = CreateWidget<UC_InventoryUIWidget>(PlayerController, InvenUiClass);
		if (InvenUI)
		{
			InvenUI->SetOwnerPlayer(Cast<AC_Player>(OwnerCharacter));
			
			if (!InvenUI->IsInViewport())
			{
				UC_Util::Print("Adding to viewport", FColor::Red, 10.f);
			}
			InvenUI->UpdateWidget();
		}
	}

	
}

void UC_InvenSystem::InitializeList()
{
	if (!IsValid(InvenUI))
	{
		InvenUI = CreateWidget<UC_InventoryUIWidget>(PlayerController, InvenUiClass);
		InvenUI->SetOwnerPlayer(Cast<AC_Player>(OwnerCharacter));

		if (!InvenUI->IsInViewport())
		{
			UC_Util::Print("Adding to viewport", FColor::Red, 10.f);
		}
	}
	
	InvenUI->UpdateWidget();
}

void UC_InvenSystem::OpenInvenUI()
{

	if (!IsValid(InvenUI)) return;	

	if (InvenUI->GetIsPanelOpened())
	{
		CloseInvenUI();
	}
	else
	{
		ShowInvenUI();
	}

	if (!IsValid(OwnerCharacter)) return;

	// TODO : 확인
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
	InvenUI->SetVisibility(ESlateVisibility::Visible);
}

void UC_InvenSystem::CloseInvenUI()
{
	if (!InvenUI)
	{  
		UC_Util::Print("From UC_InvenSystem::CloseInvenUI : InvenUI Nullptr", FColor::Red, 10.f);
		return;
	}
	InvenUI->CloseDivideItemWidget();
	InvenUI->SetVisibility(ESlateVisibility::Hidden);
}


// Called every frame
void UC_InvenSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}



