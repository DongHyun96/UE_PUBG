// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE_PUBGGameMode.h"
#include "UE_PUBGCharacter.h"
#include "Utility/C_Util.h"
#include "Character/Component/C_PlayerController.h"
#include "UObject/ConstructorHelpers.h"

AUE_PUBGGameMode::AUE_PUBGGameMode()
{
	//// 쿠킹 시작
	//// Player Pawn 
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Project_PUBG/Common/Character/BPC_Player"));
	//if (PlayerPawnBPClass.Class != nullptr)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
	//	CachedPlayerPawnClass = PlayerPawnBPClass.Class;
	//}

	//// Lobby Pawn
	//static ConstructorHelpers::FClassFinder<APawn> LobbyPawnBPClass(TEXT("/Game/Project_PUBG/Hyunho/LobbyScene/BPC_LobbyScenePawn"));
	//if (LobbyPawnBPClass.Class != nullptr)
	//{
	//	CachedLobbyPawnClass = LobbyPawnBPClass.Class;
	//}

	//PlayerControllerClass = AC_PlayerController::StaticClass();

		// ===============================
	// Player Pawn (안전하게 로드)
	// ===============================
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Project_PUBG/Common/Character/BPC_Player.BPC_Player_C"));
	if (PlayerPawnBPClass.Succeeded() && PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
		CachedPlayerPawnClass = PlayerPawnBPClass.Class;
		UE_LOG(LogTemp, Log, TEXT("✅ PlayerPawnClass loaded successfully."));
	}
	else
	{
		DefaultPawnClass = APawn::StaticClass(); // 최소 기본 Pawn으로 fallback
		CachedPlayerPawnClass = nullptr;
		UE_LOG(LogTemp, Error, TEXT("❌ Failed to load PlayerPawnClass! Check the path and packaging."));
	}

	// ===============================
	// Lobby Pawn (안전하게 로드)
	// ===============================
	static ConstructorHelpers::FClassFinder<APawn> LobbyPawnBPClass(TEXT("/Game/Project_PUBG/Hyunho/LobbyScene/BPC_LobbyScenePawn.BPC_LobbyScenePawn_C"));
	if (LobbyPawnBPClass.Succeeded() && LobbyPawnBPClass.Class != nullptr)
	{
		CachedLobbyPawnClass = LobbyPawnBPClass.Class;
		UE_LOG(LogTemp, Log, TEXT("✅ LobbyPawnClass loaded successfully."));
	}
	else
	{
		CachedLobbyPawnClass = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("⚠️ Failed to load LobbyPawnClass! Check the path or packaging."));
	}

	// ===============================
	// Player Controller
	// ===============================
	PlayerControllerClass = AC_PlayerController::StaticClass();
}

void AUE_PUBGGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	DefaultPawnClass = MapName.Contains(TEXT("LobbyMap")) ? CachedLobbyPawnClass : CachedPlayerPawnClass; 
}

