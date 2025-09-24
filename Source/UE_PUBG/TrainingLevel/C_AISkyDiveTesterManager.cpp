// Fill out your copyright notice in the Description page of Project Settings.


#include "C_AISkyDiveTesterManager.h"

#include "C_TrainingGroundManager.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
#include "Character/Component/C_InputComponent.h"
#include "Character/Component/C_PlayerController.h"
#include "CombatField/C_CombatFieldManager.h"
#include "CombatField/C_CombatFieldWidget.h"
#include "Components/BoxComponent.h"
#include "Components/CanvasPanel.h"
#include "InteractableTable/C_JumpTrainingWidget.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"


AC_AISkyDiveTesterManager::AC_AISkyDiveTesterManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_AISkyDiveTesterManager::BeginPlay()
{
	Super::BeginPlay();

	SkyDiveStartAreaBoxComponent   = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("SkyDiveStartArea")));
	SkyDiveLandingAreaBoxComponent = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("SkyDiveLandingArea")));

	if (!SkyDiveStartAreaBoxComponent)
		UC_Util::Print("From AC_AISkyDiveTesterManager::BeginPlay : SkyDiveStart Area Component not initialized!", FColor::Red, 10.f);

	if (!SkyDiveLandingAreaBoxComponent)
		UC_Util::Print("From AC_AISkyDiveTesterManager::BeginPlay : SkyDiveLanding Area Component not initialized!", FColor::Red, 10.f);

	/*UWidgetComponent* InteractionWidgetComponent = Cast<UWidgetComponent>(GetDefaultSubobjectByName(TEXT("InteractionInformWidget")));
	if (InteractionWidgetComponent)
	{
		JumpTrainingWidget = Cast<UC_JumpTrainingWidget>(InteractionWidgetComponent->GetWidget());
		if (!JumpTrainingWidget) UC_Util::Print("From AC_JumpTrainingTable::BeginPlay : JumpTrainingWidget init failed", FColor::Red, 10.f);
		else JumpTrainingWidget->SetRenderOpacity(0.f);
	}
	else UC_Util::Print("From AC_JumpTrainingTable::BeginPlay : Invalid InteractionWidgetComponent!", FColor::Red, 10.f);*/

	if (!JumpTrainingWidget)
		UC_Util::Print("From AC_AISkyDiveTesterManager::BeginPlay : JumpTrainingWidget nullptr!", FColor::Red, 10.f);
	else
	{
		JumpTrainingWidget->AddToViewport(15);
		JumpTrainingWidget->SetRenderOpacity(0.f);
	}
}

void AC_AISkyDiveTesterManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_AISkyDiveTesterManager::ToggleSpectatingSkyDiveTester()
{
	bIsSpectatingSkyDiveTester = !bIsSpectatingSkyDiveTester;
	
	UInputMappingContext* PlayerMainIMC = GAMESCENE_MANAGER->GetPlayer()->GetInputComponent()->MappingContext;

	// Spectator 관련 정보 띄우기용 Widget
	UC_CombatFieldWidget* CombatFieldWidget = GAMESCENE_MANAGER->GetTrainingGroundManager()->
	GetCombatFieldManager()->GetCombatFieldWidget(); 
	
	if (bIsSpectatingSkyDiveTester)
	{
		// Set current camera to SkyDiveTester camera
		GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(SkyDiveTesterEnemy, 0.25f);
		SkyDiveTesterEnemy->GetSpectatorCameraComponent()->Activate();

		// Remove Main IMC Player

		if (AC_PlayerController* PlayerController = GAMESCENE_MANAGER->GetPlayer()->GetController<AC_PlayerController>())
			PlayerController->RemoveIMCFromSubsystem(PlayerMainIMC);

		// Setting Spectator Info Widget
		JumpTrainingWidget->SetSpectatorInfoVisibility(true);
		return;
	}

	// Back to Player (IDLE)

	// Add Main IMC_Player
	GAMESCENE_MANAGER->GetPlayer()->GetController<AC_PlayerController>()->AddIMCToSubsystem(PlayerMainIMC, 0);

	// Set current camera to Player camera
	GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(GAMESCENE_MANAGER->GetPlayer(), 0.25f);
	
	// Spectating widget UI 감추기
	JumpTrainingWidget->SetSpectatorInfoVisibility(false);
}

FVector AC_AISkyDiveTesterManager::GetRandomLocationInBox(UBoxComponent* Box) const
{
	if (!Box)
	{
		UC_Util::Print("From AC_AISKyDiveTesterManager::GetRandomLocationInBox : Input box not valid!", FColor::Red, 10.f);
		return FVector::ZeroVector;
	}

	const FVector Origin = Box->GetComponentLocation();
	const FVector Extent = Box->GetScaledBoxExtent();

	const float RandX = FMath::FRandRange(-Extent.X, Extent.X);
	const float RandY = FMath::FRandRange(-Extent.Y, Extent.Y);
	const float RandZ = FMath::FRandRange(-Extent.Z, Extent.Z);

	// 월드 좌표로 변환
	return Origin + FVector(RandX, RandY, RandZ);
}

