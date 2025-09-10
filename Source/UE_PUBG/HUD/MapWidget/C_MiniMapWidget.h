// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Component/SkyDivingComponent/C_SkyDivingComponent.h"
#include "HUD/MapWidget/C_MapWidget.h"
#include "C_MiniMapWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_MiniMapWidget : public UC_MapWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:

	void SetVisibility(ESlateVisibility InVisibility) override;

private:

	/// <summary>
	/// GameSceneManager HUDMode MiniMap 부분 객체 Initialize 처리 
	/// </summary>
	void InitGameSceneManagerMiniMap();

	/// <summary>
	/// 매 Tick마다 Player위치에 따른 MiniMap 업데이트 처리
	/// </summary>
	void UpdatePlayerMarkerAndBGPosition();	

public:

	bool SpawnPingImage(FVector WorldPingLocation) override;

public:

	/// <summary>
	/// 핑 마커 Cancel 처리되었을 때 불려질 Callback Event
	/// </summary>
	void OnCancelPingMarker();

	void UpdateAirplaneImagePosition(const FVector& AirplaneLocation) override;

private:

	/// <summary>
	/// 맵의 Transform 또는 Player의 위치에 따른 PlaneRoute 관련 Image Transform 조정
	/// </summary>
	void HandleUpdatePlaneRouteTransform() override;

	FVector2D GetWorldToMapSizePos(FVector GameWorldLocation) override;

private:
	
	/// <summary>
	/// Player위치 OwnerPlayer World 위치에 따라 Update 시키기
	/// </summary>
	void UpdatePlayerTranslationOnMap();
	
private:

	FVector2D PlayerTranslationOnMap{};

	FVector2D PingMarkerLocalLocation{};

protected:

	UPROPERTY(meta = (BindWidget))
	UImage* ProhibitedLandImage{};

private:

	float WorldToMapImageSizeFactor{};
	const TMap<ELevelType, float> WorldToMapImageSizeFactors =
	{
		{ELevelType::ShantyTown,		0.025f},	// 2500 / 100000
		{ELevelType::TrainingGround,	0.05f},		// 2500 / 50000 
		{ELevelType::QuickStart,		0.f},		// Dummy
		{ELevelType::Max,				0.f}		// Dummy
	};
};

