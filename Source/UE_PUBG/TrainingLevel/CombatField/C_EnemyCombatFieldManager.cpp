// Fill out your copyright notice in the Description page of Project Settings.


#include "C_EnemyCombatFieldManager.h"

#include "C_CombatFieldManager.h"
#include "C_CombatFieldWidget.h"
#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"
#include "Camera/CameraComponent.h"
#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
#include "Character/Component/C_InputComponent.h"
#include "Character/Component/C_PlayerController.h"
#include "Components/BoxComponent.h"
#include "Singleton/C_GameSceneManager.h"
#include "Slate/SGameLayerManager.h"
#include "Utility/C_Util.h"


UC_EnemyCombatFieldManager::UC_EnemyCombatFieldManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_EnemyCombatFieldManager::BeginPlay()
{
	Super::BeginPlay();

	for (AC_Enemy* Enemy : CombatEnemies)
	{
		if (Enemy->GetBehaviorType() != EEnemyBehaviorType::CombatTest)
			UC_Util::Print("From UC_EnemyCombatFieldManager::BeginPlay : VersusEnemy Behavior Type is not COMBAT", FColor::Red, 10.f);

		// SpawnTransform 저장
		SpawnTransforms.Add(Enemy->GetActorTransform());

		// 죽은 뒤, Destroy될 때 Delegate 지정
		Enemy->Delegate_OnCombatCharacterDestroy.BindUObject(this, &UC_EnemyCombatFieldManager::StartRound);
	}
}


void UC_EnemyCombatFieldManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UC_EnemyCombatFieldManager::InitRound()
{
	uint8 Index{};
	
	for (AC_Enemy* Enemy : CombatEnemies)
	{
		OwnerCombatFieldManager->InitRoundForCombatCharacter(Enemy, SpawnTransforms[Index]);
		++Index;
	}
}

void UC_EnemyCombatFieldManager::StartRound()
{
	bIsPlaying = true;
	
	InitRound();

	// 서로 TargetCharacter로 잡도록 setting
	CombatEnemies[0]->GetController<AC_EnemyAIController>()->GetBehaviorComponent()->SetTargetCharacter(CombatEnemies[1]);
	CombatEnemies[1]->GetController<AC_EnemyAIController>()->GetBehaviorComponent()->SetTargetCharacter(CombatEnemies[0]);
}

void UC_EnemyCombatFieldManager::StopRound()
{
	bIsPlaying = false;
	InitRound();

	// 서로의 TargetCharacter 제거
	for (AC_Enemy* Enemy : CombatEnemies)
		Enemy->GetController<AC_EnemyAIController>()->GetBehaviorComponent()->SetTargetCharacter(nullptr);	
}

AC_Enemy* UC_EnemyCombatFieldManager::GetCurrentSpectatingEnemy() const
{
	switch (CurrentSpectatorType) { case ESpectatorType::Player: case ESpectatorType::Free: return nullptr;	}
	uint8 EnemyIndex = static_cast<uint8>(CurrentSpectatorType) - 1;

	if (EnemyIndex >= CombatEnemies.Num()) return nullptr;
	
	return CombatEnemies[EnemyIndex];
}

void UC_EnemyCombatFieldManager::ApplySpectatorChanges()
{
	switch (CurrentSpectatorType)
	{
	case ESpectatorType::Player:
	{
		UC_Util::Print("Current Type : Player", FColor::MakeRandomColor());
		
		AC_Player* Player = GAMESCENE_MANAGER->GetPlayer();
		GetWorld()->GetFirstPlayerController()->Possess(Player);
		
		// Add Main IMC_Player
		UInputMappingContext* PlayerMainIMC = GAMESCENE_MANAGER->GetPlayer()->GetInputComponent()->MappingContext;
		GAMESCENE_MANAGER->GetPlayer()->GetController<AC_PlayerController>()->AddIMCToSubsystem(PlayerMainIMC, 0);
		
		OwnerCombatFieldManager->GetCombatFieldWidget()->SetSpectatorInfoVisibility(false);
	}	
		return;
	case ESpectatorType::Enemy1: case ESpectatorType::Enemy2:
	{
		UC_Util::Print("Current Type : " + (CurrentSpectatorType == ESpectatorType::Enemy1) ? "Enemy1" : "Enemy2", FColor::MakeRandomColor());

		uint8 EnemyIndex = (CurrentSpectatorType == ESpectatorType::Enemy1) ? 0 : 1;
		if (EnemyIndex >= CombatEnemies.Num())
		{
			UC_Util::Print("From UC_EnemyCombatFieldManager::ApplySpectatorChanges : Invalid CombatTester!", FColor::Red, 10.f);
			return;
		}
		
		AC_Enemy* CombatTester = CombatEnemies[EnemyIndex];

		if (!IsValid(CombatTester->GetSpectatorCameraComponent()))
		{
			UC_Util::Print("From UC_EnemyCombatFieldManager::ApplySpectatorChanges : Invalid CombatTester's Spectator Camera", FColor::Red, 10.f);
			return;
		}

		GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(CombatTester, 0.25f);
		CombatTester->GetSpectatorCameraComponent()->Activate();
		// CombatTester->GetSpectatorCameraComponent()->Deactivate();

		// Remove Main IMC_Player
		// if (PrevSpectatorType == ESpectatorType::Free) return;
		
		UInputMappingContext* PlayerMainIMC = GAMESCENE_MANAGER->GetPlayer()->GetInputComponent()->MappingContext;
		
		if (AC_PlayerController* PlayerController = GAMESCENE_MANAGER->GetPlayer()->GetController<AC_PlayerController>())
			PlayerController->RemoveIMCFromSubsystem(PlayerMainIMC);

		OwnerCombatFieldManager->GetCombatFieldWidget()->SetSpectatorInfoVisibility(true);
		OwnerCombatFieldManager->GetCombatFieldWidget()->SetSpectatorInfoText(CurrentSpectatorType);
		
		return;
	}
	case ESpectatorType::Free:
	{
		UC_Util::Print("Current Type : Free", FColor::MakeRandomColor());

		OwnerCombatFieldManager->GetCombatFieldWidget()->SetSpectatorInfoVisibility(true);
		OwnerCombatFieldManager->GetCombatFieldWidget()->SetSpectatorInfoText(CurrentSpectatorType);
		
		// Add Main IMC_Player
		UInputMappingContext* PlayerMainIMC = GAMESCENE_MANAGER->GetPlayer()->GetInputComponent()->MappingContext;
		
		if (AC_PlayerController* PlayerController = GAMESCENE_MANAGER->GetPlayer()->GetController<AC_PlayerController>())
			PlayerController->RemoveIMCFromSubsystem(PlayerMainIMC);
			// PlayerController->AddIMCToSubsystem(PlayerMainIMC, 0);

		FVector CameraLocation{};
		FRotator CameraRotation{};
		
		GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
		FTransform CurrentCameraTransform(CameraRotation, CameraLocation);
		
		if (IsValid(FreeSpectatorPawn))
		{
			FreeSpectatorPawn->SetActorTransform(CurrentCameraTransform);
			GetWorld()->GetFirstPlayerController()->Possess(FreeSpectatorPawn);
			return;
		}

		FActorSpawnParameters SpawnParam{};
		SpawnParam.Owner = GAMESCENE_MANAGER->GetPlayer();
		SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		FreeSpectatorPawn = GetWorld()->SpawnActor<APawn>(FreeSpectatorClass, CurrentCameraTransform, SpawnParam);
		if (!IsValid(FreeSpectatorPawn))
		{
			UC_Util::Print("From UC_EnemyCombatFieldManager::ApplySpectatorChanges : FreeSpectatorPawn spawn failed!", FColor::Red, 10.f);
			return;
		}
		
		GetWorld()->GetFirstPlayerController()->Possess(FreeSpectatorPawn);
	}
	}
}
