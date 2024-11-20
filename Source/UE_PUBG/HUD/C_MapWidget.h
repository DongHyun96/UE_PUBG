// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_MapWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_MapWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	
	/// <summary>
	/// 블루프린트 쪽에서 구현한 Event Function (Map 이미지의 MainCircle Material 관련 information 조정)
	/// </summary>
	/// <param name="PosU"> : 중점의 U좌표 </param>
	/// <param name="PosV"> : 중점의 V좌표 </param>
	/// <param name="UVRad"> : 중점의 Radius(UV좌표계 단위) </param>
	UFUNCTION(BlueprintImplementableEvent)
	void SetMainCircleInfo(float PosU, float PosV, float UVRad);

	/// <summary>
	/// 블루프린트 쪽에서 구현한 Event Function (Map 이미지의 NextCircle Material 관련 information 조정)
	/// </summary>
	/// <param name="PosU"> : 중점의 U좌표 </param>
	/// <param name="PosV"> : 중점의 V좌표 </param>
	/// <param name="UVRad"> : 중점의 Radius(UV좌표계 단위) </param>
	UFUNCTION(BlueprintImplementableEvent)
	void SetNextCircleInfo(float PosU, float PosV, float UVRad);

protected:

	/// <summary>
	/// World에서 찍은 핑 마커 Spawn 처리 시키기
	/// </summary>
	/// <param name="WorldPingLocation"> : World에 찍힌 핑의 위치 </param>
	/// <returns> : Spawn이 제대로 이루어졌다면 return true </returns>
	UFUNCTION(BlueprintImplementableEvent)
	bool SpawnPingImageWithWorldPingLocation(FVector WorldPingLocation);

	/// <summary>
	/// MainMap 지도 상의 위치(MousePos)에 찍은 핑 마커 Spawn 처리 시키기
	/// </summary>
	/// <param name="MousePos"> : 찍었던 마우스 포인터 위치</param>
	/// <returns> : Spawn이 제대로 이루어졌다면 return true </returns>
	UFUNCTION(BlueprintImplementableEvent)
	bool SpawnPingImageWithMousePos(FVector2D MousePos);

public:

	/// <summary>
	/// 핑 마커 Cancel 처리되었을 때 불려질 Callback Event
	/// </summary>
	/// <returns> : Ping Marker Cancel(Hidden)이 제대로 처리되었다면 return true </returns>
	UFUNCTION(BlueprintImplementableEvent)
	bool OnCancelPingMarker();

public:

	/// <summary>
	/// World에서 찍은 핑 마커 Spawn 처리 시키기
	/// </summary>
	/// <param name="WorldPingLocation"> : World에 찍힌 핑의 위치 </param>
	/// <returns> : Spawn이 제대로 이루어졌다면 return true </returns>
	virtual bool SpawnPingImage(FVector WorldPingLocation);

	/// <summary>
	/// MainMap 지도 상의 위치(MousePos)에 찍은 핑 마커 Spawn 처리 시키기
	/// </summary>
	/// <param name="MousePos"> : 찍었던 마우스 포인터 위치</param>
	/// <returns> : Spawn이 제대로 이루어졌다면 return true </returns>
	virtual bool SpawnPingImage(FVector2D MousePos);

public:

	virtual void SetAirplaneRouteStartDestPosOrigin(TPair<FVector, FVector> StartDest);


protected:

	virtual void HandleUpdatePlaneRouteStartDest();

protected: // 비행기 경로 관련

	FVector2D AirplaneRouteStartPosOrigin{};
	FVector2D AirplaneRouteDestPosOrigin{};

	FVector2D AirplaneRouteStartPos{};
	FVector2D AirplaneRouteDestPos{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UImage* AirplaneStartCircleImage{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UImage* AirplaneDestTriangleImage{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UImage* AirplaneImg{};

private:
	const float BACKGROUND_SIZE = 2500.f;

};
