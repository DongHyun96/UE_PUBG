// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE_PUBGGameMode.h"
#include "UE_PUBGCharacter.h"
#include "Utility/C_Util.h"
#include "UObject/ConstructorHelpers.h"

AUE_PUBGGameMode::AUE_PUBGGameMode()
{
	//Change to Playable Character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Project_PUBG/Common/Character/BPC_Player"));
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Project_PUBG/Sangyun/240807BackUp/Character/BPC_Player"));
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Project_PUBG/Hyunho/Character/BPC_Player_HH"));
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Project_PUBG/DongHyun/Copy/DH_BPC_Player"));


// set default pawn class to our Blueprinted character
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

}
