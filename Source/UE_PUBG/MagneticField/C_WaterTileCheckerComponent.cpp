// Fill out your copyright notice in the Description page of Project Settings.


#include "MagneticField/C_WaterTileCheckerComponent.h"

#include "C_MagneticFieldManager.h"
#include "Singleton/C_GameSceneManager.h"

const TArray<TPair<uint8, uint8>> UC_WaterTileCheckerComponent::WATERTILE_COORDINATES = 
{
	{0, 8}, {0, 9}, {1, 6}, {1, 7}, {1, 8}, {1, 9},
	{2, 8}, {2, 9},
	{3, 9},
	{4, 6}, {4, 7}, {4 ,8}, {4, 9},
	{5, 6}, {5, 7}, {5, 8}, {5, 9},
	{6, 7}, {6, 8}, {6, 9},
	{7, 8}, {7, 9},
	{8, 0}, {8, 1}, {8, 8}, {8, 9},
	{9, 0}, {9, 1}, {9, 8}, {9, 9}
};

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

