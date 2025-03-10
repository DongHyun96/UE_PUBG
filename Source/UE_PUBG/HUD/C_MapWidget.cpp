// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/C_MapWidget.h"

#include "Utility/C_Util.h"

#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Character/C_Player.h"

#include "Character/Component/C_SkyDivingComponent.h"

#include "HUD/C_HUDWidget.h"

#include "Components/Border.h"

const float UC_MapWidget::BACKGROUND_SIZE		  = 2500.f;
const float UC_MapWidget::WORLD_TO_BG_SIZE_FACTOR = 0.025f;

void UC_MapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdatePlayerTranslationOnMap();
	UpdatePlayerMarkerRotation();

	//HandleUpdatePlaneRouteStartDest();
	HandleUpdatePlaneRouteTransform();

	
}

void UC_MapWidget::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);
	OwnerPlayer->GetHUDWidget()->GetMiniMapBorder()->SetVisibility(InVisibility);
}

bool UC_MapWidget::SpawnPingImage(FVector WorldPingLocation)
{
	return SpawnPingImageWithWorldPingLocation(WorldPingLocation);
}

bool UC_MapWidget::SpawnPingImage(FVector2D MousePos)
{
	return SpawnPingImageWithMousePos(MousePos);
}

void UC_MapWidget::SetAirplaneRoute(TPair<FVector, FVector> StartDest)
{
	// Map에 배치될 위치 지점 찾기
	AirplaneRouteStartPos = GetWorldToMapSizePos(StartDest.Key);
	AirplaneRouteDestPos = GetWorldToMapSizePos(StartDest.Value);

	// Dest triangle 회전 잡기
	if (AirplaneRouteStartPos.X - AirplaneRouteDestPos.X == 0.f)
	{
		if (AirplaneRouteStartPos.Y - AirplaneRouteDestPos.Y > 0.f)
			AirplaneDestTriangleImage->SetRenderTransformAngle(0.f);
		else AirplaneDestTriangleImage->SetRenderTransformAngle(180.f);
			
		return;
	}

	if (AirplaneRouteStartPos.Y - AirplaneRouteDestPos.Y == 0.f)
	{
		if (AirplaneRouteStartPos.X - AirplaneRouteDestPos.X > 0.f)
			AirplaneDestTriangleImage->SetRenderTransformAngle(90.f);
		else
			AirplaneDestTriangleImage->SetRenderTransformAngle(-90.f);

		return;
	}
	
	float DX = AirplaneRouteDestPos.X - AirplaneRouteStartPos.X;
	float DY = AirplaneRouteStartPos.Y - AirplaneRouteDestPos.Y;

	float Angle = FMath::RadiansToDegrees(FMath::Atan(DY / DX));
	Angle	    = (DX < 0.f) ? 270.f - Angle : 90.f - Angle;

	AirplaneDestTriangleImage->SetRenderTransformAngle(Angle);

	// Airplane 이미지 회전 잡기
	AirplaneImg->SetRenderTransformAngle(Angle - 90.f);

	// AirplaneRoute 이미지 회전 잡기
	AirplaneRouteImage->SetRenderTransformAngle(Angle - 90.f);

	// AirplaneRoute 이미지 크기 잡기
	// 부모 MapWidget에서는 크기를 한 번만 잡아주되 자식에서는(MainMapWidget) Tick에서 지속적으로 잡아주기
	if (!AirplaneRouteImageCanvasSlot) AirplaneRouteImageCanvasSlot = Cast<UCanvasPanelSlot>(AirplaneRouteImage->Slot);
	
	FVector2D NewSize	= AirplaneRouteImageCanvasSlot->GetSize();
	NewSize.X			= FVector2D::Distance(AirplaneRouteStartPos, AirplaneRouteDestPos);
	AirplaneRouteImageCanvasSlot->SetSize(NewSize);
}

void UC_MapWidget::ToggleAirplaneImageVisibility(bool Visible)
{
	ESlateVisibility ImageVisibility = Visible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden;
	AirplaneImg->SetVisibility(ImageVisibility);
}

void UC_MapWidget::UpdateAirplaneImagePosition(const FVector& AirplaneLocation)
{
	FVector2D AirplanePos = GetWorldToMapSizePos(AirplaneLocation);
	AirplanePos += PlayerTranslationOnMap;
	AirplaneImg->SetRenderTranslation(AirplanePos);
}

void UC_MapWidget::HandleUpdatePlaneRouteTransform()
{
	FVector2D StartCircleImagePos = AirplaneRouteStartPos + PlayerTranslationOnMap;
	AirplaneStartCircleImage->SetRenderTranslation(StartCircleImagePos);

	FVector2D DestTriangleImagePos = AirplaneRouteDestPos + PlayerTranslationOnMap;
	AirplaneDestTriangleImage->SetRenderTranslation(DestTriangleImagePos);

	FVector2D MidPos = (StartCircleImagePos + DestTriangleImagePos) * 0.5f;
	AirplaneRouteImage->SetRenderTranslation(MidPos);
}

FVector2D UC_MapWidget::GetWorldToMapSizePos(FVector GameWorldLocation)
{
	// Map에 배치될 위치 지점 찾기
	FVector2D Pos = { GameWorldLocation.Y, -GameWorldLocation.X };
	return Pos * WORLD_TO_BG_SIZE_FACTOR;
}

void UC_MapWidget::UpdatePlayerTranslationOnMap()
{
	if (!OwnerPlayer) return;

	FVector PlayerLocation = OwnerPlayer->GetActorLocation();
	PlayerTranslationOnMap = FVector2D(-PlayerLocation.Y, PlayerLocation.X) * WORLD_TO_BG_SIZE_FACTOR;
}

void UC_MapWidget::UpdatePlayerMarkerRotation()
{
	if (!OwnerPlayer) return;

	if (!PlayerMarkerImg)
	{
		UC_Util::Print("From UpdatePlayerMarkerRotation : PlayerMarkerImg nullptr");
		return;
	}
	
	// 회전 잡기
	bool OnAirplane = OwnerPlayer->GetMainState() == EMainState::SKYDIVING &&
		OwnerPlayer->GetSkyDivingComponent()->GetSkyDivingState() == ESkyDivingState::READY;

	float PlayerMarkerAngle = OnAirplane ? OwnerPlayer->GetController()->GetControlRotation().Yaw : OwnerPlayer->GetActorRotation().Yaw;

	PlayerMarkerImg->SetRenderTransformAngle(PlayerMarkerAngle);
}


