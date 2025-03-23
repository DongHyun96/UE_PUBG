// Fill out your copyright notice in the Description page of Project Settings.


#include "Singleton/C_GameSceneManager.h"

#include "EngineUtils.h"
#include "Character/C_Player.h"
#include "Character/C_Enemy.h"
#include "MagneticField/C_MagneticFieldManager.h"
#include "Airplane/C_AirplaneManager.h"
#include "Utility/C_Util.h"

#include "Character/Component/C_InvenSystem.h"
#include "InvenUserInterface/C_InvenUiWidget.h"

#include "HUD/C_HUDWidget.h"
#include "HUD/C_MainMapWidget.h"
#include "HUD/C_MapWidget.h"

#include "Blueprint/UserWidget.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Utility/C_TickRandomColorGenerator.h"

void UC_GameSceneManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	int DoorCount{};

	// Level에 배치된 Actor들의 BeginPlay 호출되기 이전에 객체 초기화
	for (FActorIterator Actor(&InWorld); Actor; ++Actor)
	{
		if (AC_BasicCharacter* Character = Cast<AC_BasicCharacter>(*Actor))
		{
			AllCharacters.Add(Character);
			AllCharacterActors.Add(Character);
		}

		if (AC_Player* P = Cast<AC_Player>(*Actor))
		{
			Player = P;

			HUDWidgets.Add(EHUDMode::IDLE,    Player->GetHUDWidget());
			HUDWidgets.Add(EHUDMode::INVEN,   Player->GetInvenSystem()->GetInvenUI());
			HUDWidgets.Add(EHUDMode::MAINMAP, Player->GetMainMapWidget());
			MiniMapWidget = Player->GetHUDWidget()->GetMiniMapWidget();
		}

		if (AC_Enemy* E = Cast<AC_Enemy>(*Actor)) Enemies.Add(E);

		if (AC_MagneticFieldManager* MGF_Manager = Cast<AC_MagneticFieldManager>(*Actor)) MagneticFieldManager = MGF_Manager;
		if (AC_AirplaneManager* AP_Manager = Cast<AC_AirplaneManager>(*Actor)) AirplaneManager = AP_Manager;
		if (AC_TickRandomColorGenerator* RandomColorGenerator = Cast<AC_TickRandomColorGenerator>(*Actor)) TickRandomColorGenerator = RandomColorGenerator;
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

	for (UObject* NONGCObject : GCProtectedObjects)
		NONGCObject->RemoveFromRoot();

	GCProtectedObjects.Empty();

	HUDWidgets.Empty();
	MiniMapWidget = nullptr;
}

void UC_GameSceneManager::SetCurrentHUDMode(EHUDMode InHUDMode)
{
	CurrentHUDMode = InHUDMode;

	if (!IsValid(HUDWidgets[EHUDMode::IDLE]))		HUDWidgets[EHUDMode::IDLE]    = Player->GetHUDWidget();
	if (!IsValid(HUDWidgets[EHUDMode::MAINMAP]))	HUDWidgets[EHUDMode::MAINMAP] = Player->GetMainMapWidget();
	if (!IsValid(HUDWidgets[EHUDMode::INVEN]))		HUDWidgets[EHUDMode::INVEN]   = Player->GetInvenSystem()->GetInvenUI();
	if (!IsValid(MiniMapWidget))					MiniMapWidget                 = Player->GetHUDWidget()->GetMiniMapWidget();

	switch (InHUDMode)
	{
	case EHUDMode::IDLE:
		MiniMapWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		HUDWidgets[EHUDMode::MAINMAP]->SetVisibility(ESlateVisibility::Hidden);
		HUDWidgets[EHUDMode::INVEN]->SetVisibility(ESlateVisibility::Hidden);
		Player->GetInvenSystem()->GetInvenUI()->CloseDivideItemWidget();
		HUDWidgets[EHUDMode::IDLE]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		return;
	case EHUDMode::INVEN:
		HUDWidgets[EHUDMode::IDLE]->SetVisibility(ESlateVisibility::Hidden);
		HUDWidgets[EHUDMode::MAINMAP]->SetVisibility(ESlateVisibility::Hidden);
		MiniMapWidget->SetVisibility(ESlateVisibility::Hidden);
		HUDWidgets[EHUDMode::INVEN]->SetVisibility(ESlateVisibility::Visible);
		return;
	case EHUDMode::MAINMAP:
		HUDWidgets[EHUDMode::INVEN]->SetVisibility(ESlateVisibility::Hidden);
		Player->GetInvenSystem()->GetInvenUI()->CloseDivideItemWidget();

		HUDWidgets[EHUDMode::IDLE]->SetVisibility(ESlateVisibility::Hidden);
		MiniMapWidget->SetVisibility(ESlateVisibility::Hidden);
		HUDWidgets[EHUDMode::MAINMAP]->SetVisibility(ESlateVisibility::Visible);
		return;
	case EHUDMode::MAX: default: return;
	}
}

FColor UC_GameSceneManager::GetTickRandomColor() const
{
	if (!TickRandomColorGenerator) return FColor::Red;
	return TickRandomColorGenerator->GetTickRandomColor();
}

FVector UC_GameSceneManager::ConvertTileCoordinateToTileMiddleLocation(const TPair<uint8, uint8>& TileCoordinate) const
{
	uint8 TileX = TileCoordinate.Key;
	uint8 TileY = TileCoordinate.Value;

	// (0, 0) Cell의 World Location X, Y
	float TopRightWorldX =  CELL_WORLDSIZE * 4.5f;
	float TopRightWorldY = -CELL_WORLDSIZE * 4.5f;

	float XLocation = TopRightWorldX - CELL_WORLDSIZE * TileX;
	float YLocation = TopRightWorldY + CELL_WORLDSIZE * TileY;
	
	return FVector(XLocation, YLocation, 0.f);
}

TPair<uint8, uint8> UC_GameSceneManager::GetContainingTileCoordinate(const FVector2D& WorldPositionXY) const
{
	const float X = WorldPositionXY.X;
	const float Y = WorldPositionXY.Y;
	
	FVector2D XYBorderLimits = {-CELL_WORLDSIZE * 5.f, CELL_WORLDSIZE * 5.f};

	if (X < XYBorderLimits.X || X > XYBorderLimits.Y || Y < XYBorderLimits.X || Y > XYBorderLimits.Y)
	{
		UC_Util::Print("From UC_GameSceneManager::GetContainingTileCoordinate : Parameter WorldPositionXY out of border!", FColor::Red, 10.f);
		return {};
	}

	return { (CELL_WORLDSIZE * 5.f - X) / 100, (CELL_WORLDSIZE * 5.f + Y) / 100 };
}

// TPair<uint8, uint8> Get

//void UC_GameSceneManager::OnWorldEndPlay(UWorld* InWorld)
//{
//	//UC_Util::PrintLogMessage("OnWorldEndPlay");
//
//	
//}
