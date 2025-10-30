// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE_PUBGGameMode.h"
#include "UE_PUBGCharacter.h"
#include "Utility/C_Util.h"
#include "Character/Component/C_PlayerController.h"
#include "UObject/ConstructorHelpers.h"

AUE_PUBGGameMode::AUE_PUBGGameMode()
{
	// 쿠킹 시작
	// Player Pawn 
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Project_PUBG/Common/Character/BPC_Player"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
		CachedPlayerPawnClass = PlayerPawnBPClass.Class;
	}

	// Lobby Pawn
	static ConstructorHelpers::FClassFinder<APawn> LobbyPawnBPClass(TEXT("/Game/Project_PUBG/Hyunho/LobbyScene/BPC_LobbyScenePawn"));
	if (LobbyPawnBPClass.Class != nullptr)
	{
		CachedLobbyPawnClass = LobbyPawnBPClass.Class;
	}

	PlayerControllerClass = AC_PlayerController::StaticClass();
}

void AUE_PUBGGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	DefaultPawnClass = MapName.Contains(TEXT("LobbyMap")) ? CachedLobbyPawnClass : CachedPlayerPawnClass; 
}

