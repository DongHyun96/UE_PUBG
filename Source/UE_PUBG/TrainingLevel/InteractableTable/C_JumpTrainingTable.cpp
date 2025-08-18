// Fill out your copyright notice in the Description page of Project Settings.


#include "C_JumpTrainingTable.h"

#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"
#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
#include "Character/Component/C_InputComponent.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/SkyDivingComponent/C_PlayerSkyDivingComponent.h"
#include "Components/WidgetComponent.h"
#include "HUD/C_HUDWidget.h"
#include "HUD/C_InformWidget.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "Singleton/C_GameSceneManager.h"
#include "TrainingLevel/C_AISkyDiveTesterManager.h"
#include "TrainingLevel/C_TrainingGroundManager.h"
#include "Utility/C_Util.h"

AC_JumpTrainingTable::AC_JumpTrainingTable()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_JumpTrainingTable::BeginPlay()
{
	Super::BeginPlay();

	SkyDivingStartLocation = Cast<USceneComponent>(GetDefaultSubobjectByName(TEXT("SkyDivingStartLocation")));
	if (!SkyDivingStartLocation) UC_Util::Print("From AC_JumpTrainingTable::BeginPlay : Invalid SkyDivingStartLocation!", FColor::Red, 10.f);


	UWidgetComponent* InteractionWidgetComponent = Cast<UWidgetComponent>(GetDefaultSubobjectByName(TEXT("InteractionInformWidget")));
	if (InteractionWidgetComponent)
	{
		InteractionInfoWidget = InteractionWidgetComponent->GetWidget();
		InteractionInfoWidget->SetRenderOpacity(0.f);
	}
	else UC_Util::Print("From AC_JumpTrainingTable::BeginPlay : Invalid InteractionWidgetComponent!", FColor::Red, 10.f);

	if (IsValid(GAMESCENE_MANAGER->GetPlayer()->GetInputComponent()))
	{
		GAMESCENE_MANAGER->GetPlayer()->GetInputComponent()->JumpTrainingTableNum1KeyInteractionDelegate.BindUObject(this, &AC_JumpTrainingTable::OnNumber1KeyInteraction);
		GAMESCENE_MANAGER->GetPlayer()->GetInputComponent()->JumpTrainingTableNum2KeyInteractionDelegate.BindUObject(this, &AC_JumpTrainingTable::OnNumber2KeyInteraction);
	}
}

void AC_JumpTrainingTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Set InteractionInfoWidget RenderOpacity
	InteractionInfoWidget->SetRenderOpacity(1.f - TriangleWidget->GetRenderOpacity());
}

bool AC_JumpTrainingTable::OnNumber1KeyInteraction()
{
	if (!bIsFocused) return false;
	
	AC_Player* Player = GAMESCENE_MANAGER->GetPlayer();
	
	Player->SetActorLocation(SkyDivingStartLocation->GetComponentLocation());
	Player->SetMainState(EMainState::SKYDIVING);
	Player->GetPlayerSkyDivingComponent()->SetSkyDivingState(ESkyDivingState::SKYDIVING);

	AC_EquipableItem* BackPack = Player->GetInvenComponent()->GetEquipmentItems()[EEquipSlot::BACKPACK];
	if (!BackPack) return true;

	// 배낭을 장착하고 있다면, 배낭 잠시 숨기기
	BackPack->SetActorHiddenInGame(true);
	return true;
}

bool AC_JumpTrainingTable::OnNumber2KeyInteraction()
{
	if (!bIsFocused) return false;

	AC_TrainingGroundManager* TrainingGroundManager = GAMESCENE_MANAGER->GetTrainingGroundManager();
	if (!TrainingGroundManager) return false;
	
	AC_Enemy* SkyDivingTester = TrainingGroundManager->GetAISkyDiveTesterManager()->GetSkyDiveTester();
	if (!SkyDivingTester) return false;

	if (SkyDivingTester->GetMainState() == EMainState::SKYDIVING)
	{
		GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetInformWidget()->AddPlayerWarningLog("AI SkyDiving test already in progress. Look to the skies!");
		return true;
	}

	SkyDivingTester->GetController<AC_EnemyAIController>()->GetBehaviorComponent()->SetServiceType(EServiceType::SKYDIVE);
	return true;
}
