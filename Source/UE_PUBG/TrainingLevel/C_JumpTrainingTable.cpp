// Fill out your copyright notice in the Description page of Project Settings.


#include "C_JumpTrainingTable.h"

#include "C_AISkyDiveTesterManager.h"
#include "C_TrainingGroundManager.h"
#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"
#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
#include "Character/Component/C_InputComponent.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/SkyDivingComponent/C_PlayerSkyDivingComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/WidgetComponent.h"
#include "HUD/C_HUDWidget.h"
#include "HUD/C_InformWidget.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"

TArray<AC_JumpTrainingTable*> AC_JumpTrainingTable::JumpTrainingTables{};

AC_JumpTrainingTable::AC_JumpTrainingTable()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);
}

void AC_JumpTrainingTable::BeginPlay()
{
	Super::BeginPlay();

	SkyDivingStartLocation = Cast<USceneComponent>(GetDefaultSubobjectByName(TEXT("SkyDivingStartLocation")));
	if (!SkyDivingStartLocation) UC_Util::Print("From AC_JumpTrainingTable::BeginPlay : Invalid SkyDivingStartLocation!", FColor::Red, 10.f);

	BoxColliderComponent = Cast<UShapeComponent>(GetDefaultSubobjectByName(TEXT("BoxCollider")));
	if (!BoxColliderComponent) UC_Util::Print("From AC_JumpTrainingTable::BeginPlay : Invalid BoxComponent!", FColor::Red, 10.f);

	BoxColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &AC_JumpTrainingTable::OnBoxColliderBeginOverlap);
	BoxColliderComponent->OnComponentEndOverlap.AddDynamic(this, &AC_JumpTrainingTable::OnBoxColliderEndOverlap);
	
	StaticMeshComponent = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("StaticMesh")));
	
	if (StaticMeshComponent) GetComponents<UPrimitiveComponent>(PrimitiveComponents);
	else UC_Util::Print("From AC_JumpTrainingTable::BeginPlay : Invalid StaticMeshComponent!", FColor::Red, 10.f);

	UWidgetComponent* InteractionWidgetComponent = Cast<UWidgetComponent>(GetDefaultSubobjectByName(TEXT("InteractionInformWidget")));
	if (InteractionWidgetComponent)
	{
		InteractionInfoWidget = InteractionWidgetComponent->GetWidget();
		InteractionInfoWidget->SetRenderOpacity(0.f);
	}
	else UC_Util::Print("From AC_JumpTrainingTable::BeginPlay : Invalid InteractionWidgetComponent!", FColor::Red, 10.f);

	TriangleWidgetComponent = Cast<UWidgetComponent>(GetDefaultSubobjectByName(TEXT("TriangleWidget")));
	if (TriangleWidgetComponent)
	{
		TriangleWidget = TriangleWidgetComponent->GetWidget();
		TriangleWidget->SetRenderOpacity(1.f);
	}
	else UC_Util::Print("From AC_JumpTrainingTable::BeginPlay : Invalid TriangleWidgetComponent!", FColor::Red, 10.f); 

	JumpTrainingTables.Add(this);

	if (IsValid(GAMESCENE_MANAGER->GetPlayer()->GetInputComponent()))
	{
		UC_Util::Print("Player InputComponent Valid in Actor BeginPlay", FColor::MakeRandomColor(), 10.f);
		GAMESCENE_MANAGER->GetPlayer()->GetInputComponent()->JumpTrainingTableNum1KeyInteractionDelegate.BindStatic(&AC_JumpTrainingTable::OnNumber1KeyInteraction);
		GAMESCENE_MANAGER->GetPlayer()->GetInputComponent()->JumpTrainingTableNum2KeyInteractionDelegate.BindUObject(this, &AC_JumpTrainingTable::OnNumber2KeyInteraction);
	}
}

void AC_JumpTrainingTable::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (!JumpTrainingTables.IsEmpty())
		JumpTrainingTables.Empty();
}

void AC_JumpTrainingTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float InteractionInfoWidgetOpacity = FMath::Lerp(InteractionInfoWidget->GetRenderOpacity(), InteractionInfoWidgetOpacityDestination, DeltaTime * 10.f);
	InteractionInfoWidget->SetRenderOpacity(InteractionInfoWidgetOpacity);
	TriangleWidget->SetRenderOpacity(1.f - InteractionInfoWidgetOpacity);

	if (TriangleWidget->GetRenderOpacity() > 0.f)
	{
		FRotator Rotation = TriangleWidgetComponent->GetRelativeRotation();
		Rotation.Yaw += 120.f * DeltaTime;
		TriangleWidgetComponent->SetRelativeRotation(Rotation);
	}
}

void AC_JumpTrainingTable::OnBoxColliderBeginOverlap
(
	UPrimitiveComponent*	OverlappedComp,
	AActor*					OtherActor,
	UPrimitiveComponent*	OtherComp,
	int32					OtherBodyIndex,
	bool					bFromSweep,
	const FHitResult&		SweepResult
)
{
	AC_Player* Player = Cast<AC_Player>(OtherActor);
	if (!Player) return;

	// Focused effect 켜기
	for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
	{
		if (!PrimitiveComponent) continue;
		PrimitiveComponent->SetRenderCustomDepth(true);
		PrimitiveComponent->SetCustomDepthStencilValue(1);
		
		InteractionInfoWidgetOpacityDestination = 1.f;
		
		bIsFocused = true;
	}
}

void AC_JumpTrainingTable::OnBoxColliderEndOverlap
(
	UPrimitiveComponent*	OverlappedComp,
	AActor*					OtherActor,
	UPrimitiveComponent*	OtherComp,
	int32					OtherBodyIndex
)
{
	AC_Player* Player = Cast<AC_Player>(OtherActor);
	if (!Player) return;
	
	// Focused Effect 끄기
	for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
	{
		if (!PrimitiveComponent) continue;
		PrimitiveComponent->SetRenderCustomDepth(false);
		PrimitiveComponent->SetCustomDepthStencilValue(0);
		
		InteractionInfoWidgetOpacityDestination = 0.f;
		
		bIsFocused = false;
	}
}

bool AC_JumpTrainingTable::OnNumber1KeyInteraction()
{
	for (AC_JumpTrainingTable* Table : JumpTrainingTables)
	{
		if (Table->bIsFocused)
		{
			AC_Player* Player = UC_GameSceneManager::GetInstance(Table->GetWorld())->GetPlayer();
			
			Player->SetActorLocation(Table->SkyDivingStartLocation->GetComponentLocation());
			Player->SetMainState(EMainState::SKYDIVING);
			Player->GetPlayerSkyDivingComponent()->SetSkyDivingState(ESkyDivingState::SKYDIVING);

			AC_EquipableItem* BackPack = Player->GetInvenComponent()->GetEquipmentItems()[EEquipSlot::BACKPACK];
			if (!BackPack) return true;

			// 배낭을 장착하고 있다면, 배낭 잠시 숨기기S
			BackPack->SetActorHiddenInGame(true);
			return true;
		}
	}
	return false;
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
