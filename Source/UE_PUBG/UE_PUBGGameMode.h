// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

	#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UE_PUBGGameMode.generated.h"

UCLASS(minimalapi)
class AUE_PUBGGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AUE_PUBGGameMode();
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	// virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

private:

	TSubclassOf<APawn> CachedPlayerPawnClass{};	
	TSubclassOf<APawn> CachedLobbyPawnClass{};	
	
};





