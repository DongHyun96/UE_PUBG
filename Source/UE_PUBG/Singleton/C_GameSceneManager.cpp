// Fill out your copyright notice in the Description page of Project Settings.


#include "Singleton/C_GameSceneManager.h"

#include "EngineUtils.h"
#include "Character/C_Player.h"
#include "MagneticField/C_MagneticFieldManager.h"
#include "Airplane/C_AirplaneManager.h"
#include "Utility/C_Util.h"

#include "Character/Component/C_InvenSystem.h"
#include "InvenUserInterface/C_InvenUiWidget.h"

#include "HUD/C_HUDWidget.h"
#include "HUD/C_MainMapWidget.h"
#include "HUD/C_MapWidget.h"

#include "Blueprint/UserWidget.h"

void UC_GameSceneManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// Level에 배치된 Actor들의 BeginPlay 호출되기 이전에 객체 초기화
	for (FActorIterator Actor(&InWorld); Actor; ++Actor)
	{
		if (AC_Player* P = Cast<AC_Player>(*Actor))
		{
			Player = P;

			HUDWidgets.Add(EHUDMode::IDLE,    Player->GetHUDWidget());
			HUDWidgets.Add(EHUDMode::INVEN,   Player->GetInvenSystem()->GetInvenUI());
			HUDWidgets.Add(EHUDMode::MAINMAP, Player->GetHUDWidget()->GetMainMapWidget());
			MiniMapWidget = Player->GetHUDWidget()->GetMiniMapWidget();

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

	HUDWidgets.Empty();
}

void UC_GameSceneManager::SetCurrentHUDMode(EHUDMode InHUDMode)
{
	CurrentHUDMode = InHUDMode;

	if (!IsValid(HUDWidgets[EHUDMode::IDLE]))		HUDWidgets[EHUDMode::IDLE]    = Player->GetHUDWidget();
	if (!IsValid(HUDWidgets[EHUDMode::MAINMAP]))	HUDWidgets[EHUDMode::MAINMAP] = Player->GetHUDWidget()->GetMainMapWidget();
	if (!IsValid(HUDWidgets[EHUDMode::INVEN]))		HUDWidgets[EHUDMode::INVEN]   = Player->GetInvenSystem()->GetInvenUI();
	if (!IsValid(MiniMapWidget))					MiniMapWidget                 = Player->GetHUDWidget()->GetMiniMapWidget();

	switch (InHUDMode)
	{
	case EHUDMode::IDLE:
		HUDWidgets[EHUDMode::IDLE]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		MiniMapWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		HUDWidgets[EHUDMode::MAINMAP]->SetVisibility(ESlateVisibility::Hidden);
		HUDWidgets[EHUDMode::INVEN]->SetVisibility(ESlateVisibility::Hidden);
		return;
	case EHUDMode::INVEN:
		HUDWidgets[EHUDMode::IDLE]->SetVisibility(ESlateVisibility::Hidden);
		HUDWidgets[EHUDMode::MAINMAP]->SetVisibility(ESlateVisibility::Hidden);
		MiniMapWidget->SetVisibility(ESlateVisibility::Hidden);

		HUDWidgets[EHUDMode::INVEN]->SetVisibility(ESlateVisibility::Visible);
		return;
	case EHUDMode::MAINMAP:
		HUDWidgets[EHUDMode::INVEN]->SetVisibility(ESlateVisibility::Hidden);
		HUDWidgets[EHUDMode::IDLE]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		HUDWidgets[EHUDMode::MAINMAP]->SetVisibility(ESlateVisibility::Visible);

		MiniMapWidget->SetVisibility(ESlateVisibility::Hidden);
		return;
	case EHUDMode::MAX: default: return;
	}
}

//void UC_GameSceneManager::OnWorldEndPlay(UWorld* InWorld)
//{
//	//UC_Util::PrintLogMessage("OnWorldEndPlay");
//
//	
//}
