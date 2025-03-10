// Fill out your copyright notice in the Description page of Project Settings.


#include "MagneticField/C_WaterTileCheckerComponent.h"

#include "C_MagneticFieldManager.h"

UC_WaterTileCheckerComponent::UC_WaterTileCheckerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UC_WaterTileCheckerComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UC_WaterTileCheckerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

uint8 UC_WaterTileCheckerComponent::GetWaterTileCount(const FMagneticCircle& TargetCircle) const
{
	uint8 Count{};
	
	for (const TPair<uint8, uint8>& WaterTileCoord : WATERTILE_COORDINATES)
	{
		FVector TileLocation = ConvertTileCoordinateToTileMiddleLocation(WaterTileCoord);

		float Distance = FVector::Distance(TileLocation, TargetCircle.MidLocation);
		if (Distance < TargetCircle.Radius) ++Count;
	}
	
	return Count;
}

FVector UC_WaterTileCheckerComponent::ConvertTileCoordinateToTileMiddleLocation(const TPair<uint8, uint8>& TileCoordinate) const
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

