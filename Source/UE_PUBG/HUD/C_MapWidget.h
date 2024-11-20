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
	/// �������Ʈ �ʿ��� ������ Event Function (Map �̹����� MainCircle Material ���� information ����)
	/// </summary>
	/// <param name="PosU"> : ������ U��ǥ </param>
	/// <param name="PosV"> : ������ V��ǥ </param>
	/// <param name="UVRad"> : ������ Radius(UV��ǥ�� ����) </param>
	UFUNCTION(BlueprintImplementableEvent)
	void SetMainCircleInfo(float PosU, float PosV, float UVRad);

	/// <summary>
	/// �������Ʈ �ʿ��� ������ Event Function (Map �̹����� NextCircle Material ���� information ����)
	/// </summary>
	/// <param name="PosU"> : ������ U��ǥ </param>
	/// <param name="PosV"> : ������ V��ǥ </param>
	/// <param name="UVRad"> : ������ Radius(UV��ǥ�� ����) </param>
	UFUNCTION(BlueprintImplementableEvent)
	void SetNextCircleInfo(float PosU, float PosV, float UVRad);

protected:

	/// <summary>
	/// World���� ���� �� ��Ŀ Spawn ó�� ��Ű��
	/// </summary>
	/// <param name="WorldPingLocation"> : World�� ���� ���� ��ġ </param>
	/// <returns> : Spawn�� ����� �̷�����ٸ� return true </returns>
	UFUNCTION(BlueprintImplementableEvent)
	bool SpawnPingImageWithWorldPingLocation(FVector WorldPingLocation);

	/// <summary>
	/// MainMap ���� ���� ��ġ(MousePos)�� ���� �� ��Ŀ Spawn ó�� ��Ű��
	/// </summary>
	/// <param name="MousePos"> : ����� ���콺 ������ ��ġ</param>
	/// <returns> : Spawn�� ����� �̷�����ٸ� return true </returns>
	UFUNCTION(BlueprintImplementableEvent)
	bool SpawnPingImageWithMousePos(FVector2D MousePos);

public:

	/// <summary>
	/// �� ��Ŀ Cancel ó���Ǿ��� �� �ҷ��� Callback Event
	/// </summary>
	/// <returns> : Ping Marker Cancel(Hidden)�� ����� ó���Ǿ��ٸ� return true </returns>
	UFUNCTION(BlueprintImplementableEvent)
	bool OnCancelPingMarker();

public:

	/// <summary>
	/// World���� ���� �� ��Ŀ Spawn ó�� ��Ű��
	/// </summary>
	/// <param name="WorldPingLocation"> : World�� ���� ���� ��ġ </param>
	/// <returns> : Spawn�� ����� �̷�����ٸ� return true </returns>
	virtual bool SpawnPingImage(FVector WorldPingLocation);

	/// <summary>
	/// MainMap ���� ���� ��ġ(MousePos)�� ���� �� ��Ŀ Spawn ó�� ��Ű��
	/// </summary>
	/// <param name="MousePos"> : ����� ���콺 ������ ��ġ</param>
	/// <returns> : Spawn�� ����� �̷�����ٸ� return true </returns>
	virtual bool SpawnPingImage(FVector2D MousePos);

public:

	virtual void SetAirplaneRouteStartDestPosOrigin(TPair<FVector, FVector> StartDest);


protected:

	virtual void HandleUpdatePlaneRouteStartDest();

protected: // ����� ��� ����

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
