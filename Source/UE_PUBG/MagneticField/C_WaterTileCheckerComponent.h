// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_WaterTileCheckerComponent.generated.h"


struct FMagneticCircle;

// TODO : Enemy 낙하지점 정할 때 물인지 체크

/// <summary>
/// <para> Level에서 특정 위치가 물인지 판별하는 WaterTileChecker 클래스 </para>
/// <para> Shanty-town 맵 기준 우측 상단 Tile 좌표(0,0), 좌측 하단 Tile 좌표(9,9) | 타일 크기는 100 x 100(m)로 잡음 </para>
/// </summary>
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_WaterTileCheckerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UC_WaterTileCheckerComponent();

protected:
	
	virtual void BeginPlay() override;

public:
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetMagneticFieldManager(class AC_MagneticFieldManager* InMagneticFieldManager) { MagneticFieldManager = InMagneticFieldManager; }

	/// <summary>
	/// <para> TargetCircle에 포함된 WaterTile 개수를 대략적으로 계산하여 구하는 함수 </para>
	/// <para> TargetCircle 중점으로부터 WaterTile 중심까지의 거리가 반지름 이내라면 해당 WaterTile을 포함한다고 침 </para>
	/// </summary>
	/// <returns> Water Tile 개수 </returns>
	uint8 GetWaterTileCount(const FMagneticCircle& TargetCircle) const;


	/*/// <summary>
	/// TargetLocation이 Water인지 아닌지 WaterTile 위치와 TargetPhaseRadius를 통해 대략적으로 판별
	/// </summary>
	/// <param name="TargetLocation"> : 검사할 위치(World Location) </param>
	/// <param name="TargetPhaseRadius"> : </param>
	/// <returns></returns>
	bool IsWater(const FVector& TargetLocation, const float& TargetPhaseRadius);*/
	
private:
	
	/// <summary>
	/// 해당 Tile의 중앙점을 World 좌표로 환산한 값 구하기, Z는 항상 0.f
	/// </summary>
	/// <param name="TileCoordinate"></param>
	/// <returns></returns>
	FVector ConvertTileCoordinateToTileMiddleLocation(const TPair<uint8, uint8>& TileCoordinate) const;
		
private:
	
	class AC_MagneticFieldManager* MagneticFieldManager{};

private:

	// Water로 지정된 Tile 좌표들
	const TArray<TPair<uint8, uint8>> WATERTILE_COORDINATES =
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

	const float CELL_WORLDSIZE = 10000.f;
};
