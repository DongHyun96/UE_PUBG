// Fill out your copyright notice in the Description page of Project Settings.


#include "Singleton/C_GameSceneManager.h"

#include "EngineUtils.h"
#include "Character/C_Player.h"
#include "MagneticField/C_MagneticFieldManager.h"
#include "Utility/C_Util.h"

void UC_GameSceneManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// Level에 배치된 Actor들의 BeginPlay 호출되기 이전에 객체 초기화
	for (FActorIterator Actor(&InWorld); Actor; ++Actor)
	{
		if (AC_Player* P = Cast<AC_Player>(*Actor))
		{
			Player = P;
			UC_Util::Print(Player, FColor::Red, 10.f);
		}

		if (AC_MagneticFieldManager* MGF_Manager = Cast<AC_MagneticFieldManager>(*Actor)) MagneticFieldManager = MGF_Manager;
	}
}
