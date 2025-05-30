﻿// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE_PUBGGameMode.h"
#include "UE_PUBGCharacter.h"
#include "Utility/C_Util.h"
#include "Character/Component/C_PlayerController.h"
#include "UObject/ConstructorHelpers.h"

AUE_PUBGGameMode::AUE_PUBGGameMode()
{
	//Change to Playable Character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Project_PUBG/Common/Character/BPC_Player"));
	static ConstructorHelpers::FClassFinder<APawn> LobbyPawnClass(TEXT("/Game/Project_PUBG/Hyunho/LobbyScene/BPC_LobbyScenePawn"));

	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Project_PUBG/Sangyun/240807BackUp/Character/BPC_Player"));
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Project_PUBG/Hyunho/Character/BPC_Player_HH"));
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Project_PUBG/DongHyun/Copy/DH_BPC_Player"));
	if (GetWorld())
	{
		if (GetWorld()->GetMapName().Contains(TEXT("LobbyMap")))
		{
			if (LobbyPawnClass.Class != NULL)
				DefaultPawnClass = LobbyPawnClass.Class;
			PlayerControllerClass = AC_PlayerController::StaticClass();

		}
		else
		{			
		
			// set default pawn class to our Blueprinted character
			if (PlayerPawnBPClass.Class != NULL)
			{
				DefaultPawnClass = PlayerPawnBPClass.Class;
				PlayerControllerClass = AC_PlayerController::StaticClass();
			}
		}

	}

}

/*
void AUE_PUBGGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	
	if (MapName.Contains(TEXT("LobbyMap")))
	{
		static ConstructorHelpers::FClassFinder<APawn> LobbyPawnClass(TEXT("/Game/Project_PUBG/Hyunho/LobbyScene/BPC_LobbyScenePawn"));
		if (LobbyPawnClass.Class)
		{
			DefaultPawnClass = LobbyPawnClass.Class;
		}
	}
	else
	{
		static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Project_PUBG/Common/Character/BPC_Player"));
		if (PlayerPawnBPClass.Class)
		{
			DefaultPawnClass = PlayerPawnBPClass.Class;
		}
	}

	PlayerControllerClass = AC_PlayerController::StaticClass();
}
*/


