// Fill out your copyright notice in the Description page of Project Settings.


#include "C_MiniMapWidget.h"

#include "Character/C_Player.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "HUD/C_HUDWidget.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"

void UC_MiniMapWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// GameSceneManager HUDMode MiniMap 부분 객체 Initialize 처리
	GAMESCENE_MANAGER->SetMiniMapWidget(this);

	// 현재 Level 지도의 축척에 따른 원점값(맵 중앙 위치) setting
	WorldToMapImageSizeFactor = WorldToMapImageSizeFactors[CurrentSelectedMap];
	
	/*// 현재 Level 축척에 따른 GridImage size 조정
	UCanvasPanelSlot* GridImagePanelSlot = Cast<UCanvasPanelSlot>(GridImage->Slot);
	if (!GridImagePanelSlot)
	{
		UC_Util::Print("From UC_MiniMapWidget::NativeConstruct : GridImagePanelSlot casting failed", FColor::Red, 10.f);
		return;
	}

	const float GridImageSize = CurrentSelectedMap == ELevelType::TrainingGround ? 5000.f : 2500.f;
	GridImagePanelSlot->SetSize(FVector2D(GridImageSize));*/
}

void UC_MiniMapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!OwnerPlayer) return;
	
	UpdatePlayerTranslationOnMap();
	UpdatePlayerMarkerAndBGPosition();
}

void UC_MiniMapWidget::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);
	OwnerPlayer->GetHUDWidget()->GetMiniMapBorder()->SetVisibility(InVisibility);
}

void UC_MiniMapWidget::UpdatePlayerMarkerAndBGPosition()
{
	// PlayerMarkerPos가 현재의 중앙이 됨 -> 맵 이미지를 Player의 위치에 맞춤
	GridImage->SetRenderTranslation(PlayerTranslationOnMap);
	ProhibitedLandImage->SetRenderTranslation(PlayerTranslationOnMap);
	MapImage->SetRenderTranslation(PlayerTranslationOnMap);

	const FVector2D PingLocation = PingMarkerLocalLocation + PlayerTranslationOnMap; 
	PingBorder->SetRenderTranslation(PingLocation);	
}

bool UC_MiniMapWidget::SpawnPingImage(FVector WorldPingLocation)
{
	if (!PingBorder) return false;
	WorldPingLocation -= OriginLocation;
	PingMarkerLocalLocation.X =  WorldPingLocation.Y * WorldToMapImageSizeFactor;
	PingMarkerLocalLocation.Y = -WorldPingLocation.X * WorldToMapImageSizeFactor;

	PingBorder->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	return true;
}

void UC_MiniMapWidget::OnCancelPingMarker()
{
	PingBorder->SetVisibility(ESlateVisibility::Hidden);
}

void UC_MiniMapWidget::UpdateAirplaneImagePosition(const FVector& AirplaneLocation)
{
	FVector2D AirplanePos = GetWorldToMapSizePos(AirplaneLocation);
	AirplanePos += PlayerTranslationOnMap;
	AirplaneImage->SetRenderTranslation(AirplanePos);
}

void UC_MiniMapWidget::HandleUpdatePlaneRouteTransform()
{
	const FVector2D StartCircleImagePos = AirplaneRouteStartPos + PlayerTranslationOnMap;
	AirplaneStartCircle->SetRenderTranslation(StartCircleImagePos);

	const FVector2D DestTriangleImagePos = AirplaneRouteDestPos + PlayerTranslationOnMap;
	AirplaneDestTriangle->SetRenderTranslation(DestTriangleImagePos);

	const FVector2D MidPos = (StartCircleImagePos + DestTriangleImagePos) * 0.5f;
	AirplaneRouteLine->SetRenderTranslation(MidPos);
}

FVector2D UC_MiniMapWidget::GetWorldToMapSizePos(FVector GameWorldLocation)
{
	GameWorldLocation -= OriginLocation;
	
	// Map에 배치될 위치 지점 찾기
	const FVector2D Pos = { GameWorldLocation.Y, -GameWorldLocation.X };
	return Pos * WorldToMapImageSizeFactor;
}

void UC_MiniMapWidget::UpdatePlayerTranslationOnMap()
{
	const FVector PlayerLocation = OwnerPlayer->GetActorLocation() - OriginLocation;
	PlayerTranslationOnMap = FVector2D(-PlayerLocation.Y, PlayerLocation.X) * WorldToMapImageSizeFactor;
}
