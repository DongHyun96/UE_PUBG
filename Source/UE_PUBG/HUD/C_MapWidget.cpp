// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/C_MapWidget.h"

#include "Utility/C_Util.h"

void UC_MapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	HandleUpdatePlaneRouteStartDest();
}

bool UC_MapWidget::SpawnPingImage(FVector WorldPingLocation)
{
	return SpawnPingImageWithWorldPingLocation(WorldPingLocation);
}

bool UC_MapWidget::SpawnPingImage(FVector2D MousePos)
{
	return SpawnPingImageWithMousePos(MousePos);
}

void UC_MapWidget::SetAirplaneRouteStartDestPosOrigin(TPair<FVector, FVector> StartDest)
{
	AirplaneRouteStartPosOrigin = { StartDest.Key.Y,   -StartDest.Key.X };
	AirplaneRouteDestPosOrigin  = { StartDest.Value.Y, -StartDest.Value.X };

	//AirplaneRouteStartPosOrigin *= (CANVAS_SIZE / WORLD_MAP_SIZE);
	//AirplaneRouteDestPosOrigin  *= (CANVAS_SIZE / WORLD_MAP_SIZE);
}

void UC_MapWidget::HandleUpdatePlaneRouteStartDest()
{

}
