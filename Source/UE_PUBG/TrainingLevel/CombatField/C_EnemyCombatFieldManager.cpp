// Fill out your copyright notice in the Description page of Project Settings.


#include "C_EnemyCombatFieldManager.h"

#include "C_CombatFieldManager.h"
#include "Camera/CameraComponent.h"
#include "Character/C_Player.h"
#include "Character/Component/C_PlayerController.h"
#include "Components/BoxComponent.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"


UC_EnemyCombatFieldManager::UC_EnemyCombatFieldManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_EnemyCombatFieldManager::BeginPlay()
{
	Super::BeginPlay();
}


void UC_EnemyCombatFieldManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UC_EnemyCombatFieldManager::IncreaseSpectatorType()
{
	ESpectatorType PrevType = CurrentSpectatorType;
	
	switch (++CurrentSpectatorType)
	{
	case ESpectatorType::Player:
	{
		UC_Util::Print("Current Type : Player", FColor::MakeRandomColor());
		
		AC_Player* Player = GAMESCENE_MANAGER->GetPlayer();
		GetWorld()->GetFirstPlayerController()->Possess(Player);

		if (PrevType == ESpectatorType::Free)
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
			{
				FreeSpectatorPawn->Destroy();
				FreeSpectatorPawn = nullptr;
			});
		}
	}	
		break;
	case ESpectatorType::Enemy1:
		UC_Util::Print("Current Type : Enemy1", FColor::MakeRandomColor());
		break;
	case ESpectatorType::Enemy2:
		UC_Util::Print("Current Type : Enemy2", FColor::MakeRandomColor());
		break;
	case ESpectatorType::Free:
	{
		UC_Util::Print("Current Type : Free", FColor::MakeRandomColor());
		AC_Player* Player = GAMESCENE_MANAGER->GetPlayer();
		FTransform PlayerCameraTransform = Player->GetMainCamera()->GetComponentTransform();

		FActorSpawnParameters SpawnParam{};
		SpawnParam.Owner = Player;
		SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		// Testing 용으로 Free 시점으로 바로 처리해보기
		FreeSpectatorPawn = GetWorld()->SpawnActor<APawn>(FreeSpectatorPawnClass, PlayerCameraTransform, SpawnParam);
		if (!IsValid(FreeSpectatorPawn))
		{
			UC_Util::Print("From UC_EnemyCombatFieldManager : FreeSpectatorPawn spawn failed!", FColor::Red, 10.f);
			return;
		}
		
		GetWorld()->GetFirstPlayerController()->Possess(FreeSpectatorPawn);
		break;
	}
	}
}

void UC_EnemyCombatFieldManager::DecreaseSpectatorType()
{
	/*ESpectatorType PrevType = CurrentSpectatorType;
	
	switch (--CurrentSpectatorType)
	{
	case ESpectatorType::Player:
	{
		AC_Player* Player = GAMESCENE_MANAGER->GetPlayer();
		AC_PlayerController* PlayerController = Player->GetController<AC_PlayerController>();
		PlayerController->Possess(Player);

		if (PrevType == ESpectatorType::Free)
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
			{
				FreeSpectatorPawn->Destroy();
				FreeSpectatorPawn = nullptr;
			});
		}
	}	
		break;
	case ESpectatorType::Enemy1:
		break;
	case ESpectatorType::Enemy2:
		break;
	case ESpectatorType::Free:
	{
		AC_Player* Player = GAMESCENE_MANAGER->GetPlayer();
		FTransform PlayerCameraTransform = Player->GetMainCamera()->GetComponentTransform();

		FActorSpawnParameters SpawnParam{};
		SpawnParam.Owner = Player;
		SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		// Testing 용으로 Free 시점으로 바로 처리해보기
		FreeSpectatorPawn = GetWorld()->SpawnActor<APawn>(FreeSpectatorPawnClass, PlayerCameraTransform, SpawnParam);
		if (!IsValid(FreeSpectatorPawn))
		{
			UC_Util::Print("From UC_EnemyCombatFieldManager : FreeSpectatorPawn spawn failed!", FColor::Red, 10.f);
			return;
		}

		AC_PlayerController* PlayerController = Player->GetController<AC_PlayerController>();
		PlayerController->Possess(FreeSpectatorPawn);
		break;
	}
	}*/
}
