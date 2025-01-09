// Fill out your copyright notice in the Description page of Project Settings.


#include "Singleton/C_GameSceneManager.h"

#include "EngineUtils.h"
#include "Character/C_Player.h"
#include "MagneticField/C_MagneticFieldManager.h"
#include "Airplane/C_AirplaneManager.h"
#include "Utility/C_Util.h"

#include "HUD/C_HUDWidget.h"
#include "HUD/C_MainMapWidget.h"

void UC_GameSceneManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// Level에 배치된 Actor들의 BeginPlay 호출되기 이전에 객체 초기화
	for (FActorIterator Actor(&InWorld); Actor; ++Actor)
	{
		if (AC_Player* P = Cast<AC_Player>(*Actor))
		{
			Player = P;
			//UC_Util::Print(Player, FColor::Red, 10.f);
			AllCharacters.Add(Player); // TODO : AllCharacters Enemy들 add시키기
		}

		if (AC_MagneticFieldManager* MGF_Manager = Cast<AC_MagneticFieldManager>(*Actor)) MagneticFieldManager = MGF_Manager;
		if (AC_AirplaneManager* AP_Manager = Cast<AC_AirplaneManager>(*Actor)) AirplaneManager = AP_Manager;
	}
}

void UC_GameSceneManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// 월드 파괴 전 호출되는 델리게이트 등록
	//FWorldDelegates::OnPreWorldFinishDestroy.AddUObject(this, &UC_GameSceneManager::OnWorldEndPlay);
}

void UC_GameSceneManager::Deinitialize()
{
	Super::Deinitialize();

	for (UObject* NONGCObject : NonGCObjects)
		NONGCObject->RemoveFromRoot();

	NonGCObjects.Empty();

}

//void UC_GameSceneManager::OnWorldEndPlay(UWorld* InWorld)
//{
//	//UC_Util::PrintLogMessage("OnWorldEndPlay");
//
//	
//}
