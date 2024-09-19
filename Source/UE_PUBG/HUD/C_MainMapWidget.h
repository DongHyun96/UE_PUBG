// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUD/C_MapWidget.h"
#include "C_MainMapWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_MainMapWidget : public UC_MapWidget
{
	GENERATED_BODY()

public:
	void NativeConstruct() override;

	void OnMKey();

protected:

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:

	void HandleUpdateMainMapImage(float InDeltaTime);

	void HandleUpdateMarkers();

private:

	//FEventReply OnMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent) override;
	FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:

	bool SpawnPingImage(FVector WorldPingLocation) override;
	bool SpawnPingImage(FVector2D MousePos) override;

protected:

	bool CancelPingMarker();

public:

	void SetPlayer(class AC_Player* InPlayer) { Player = InPlayer; }

private:

	/// <summary>
	/// LMB Down Input handling
	/// </summary>
	/// <param name="InGeometry"></param>
	/// <param name="InMouseEvent"></param>
	/// <returns> : return true if handled correctly </returns>
	bool HandleLMBDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);

	/// <summary>
	/// RMB Down Input handling
	/// </summary>
	/// <param name="InGeometry"></param>
	/// <param name="InMouseEvent"></param>
	/// <returns> : return true if handled correctly </returns>
	bool HandleRMBDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);

protected:
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UImage* MainMapImg{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UImage* PlayerMarkerImg{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UImage* GridImg{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UBorder* PingMarkerBorder{};

private:

	float MainMapScaleLerpDest = 1.f;
	FVector2D MainMapPosLerpDest{};

	float MainMapScale = 1.f;

private:

	FVector2D PingMarkerPos{};

private:

	const float		MAP_SCALE_MAX		= 5.f;
	const float		SCROLL_DELTA_STEP	= 0.5f;
	const float		CANVAS_SIZE			= 1080.f;
	const FVector2D MID_POINT			= {960.f, 540.f};
	const float		WORLD_MAP_SIZE		= 100000.f;

	const float		FIXED_LANDSCAPE_HEIGHT	= 3400.f;

	const float		PING_BORDER_SIZE	= 30.f;

private:

	bool bIsMouseDragging{};
	FVector2D PrevDragMousePos{};
	const float DRAG_SPEED = 5.f;

private:

	class AC_Player* Player{};
};
