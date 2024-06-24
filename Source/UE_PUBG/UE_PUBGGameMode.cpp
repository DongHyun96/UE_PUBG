// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE_PUBGGameMode.h"
#include "UE_PUBGCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUE_PUBGGameMode::AUE_PUBGGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Project_PUBG/Common/Character/BPC_Player"));

	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
