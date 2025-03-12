// Fill out your copyright notice in the Description page of Project Settings.


#include "MagneticField/C_WaterTileCheckerComponent.h"

#include "C_MagneticFieldManager.h"
#include "Singleton/C_GameSceneManager.h"

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
		FVector TileLocation = GAMESCENE_MANAGER->ConvertTileCoordinateToTileMiddleLocation(WaterTileCoord);

		float Distance = FVector::Distance(TileLocation, TargetCircle.MidLocation);
		if (Distance < TargetCircle.Radius) ++Count;
	}
	
	return Count;
}

