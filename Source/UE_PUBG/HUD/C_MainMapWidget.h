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

	void SetVisibility(ESlateVisibility InVisibility) override;

protected:

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:

	void HandleUpdateMainMapImage(float InDeltaTime);

	/// <summary>
	/// MainMap Transform에 따른 Map요소들 위치 Update
	/// </summary>
	void HandleUpdateMarkers();

	/// <summary>
	/// 맵의 Transform 또는 Player의 위치에 따른 PlaneRoute 관련 Image Transform 조정
	/// </summary>
	void HandleUpdatePlaneRouteTransform() override;

private:

	//FEventReply OnMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent) override;
	FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)	override;
	FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)		override;
	FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)	override;

	FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)		override;

public:

	/// <summary>
	/// 외부에서(World에서 핑을 찍었을 때) MainMapWidget에서만 SpawnPingImage 처리 시키기
	/// </summary>
	/// <param name="WorldPingLocation"> : Spawn된 WorldPing 위치 </param>
	/// <returns> : 제대로 Spawn 되었다면 return true </returns>
	bool SpawnPingImage(FVector WorldPingLocation) override;

	/// <summary>
	/// MainMap 이미지 내에서 Ping Marker를 Spawn시켰을 때의 처리(다른 곳에도 Ping Marker 스폰시키기)
	/// </summary>
	/// <param name="MousePos"> : Button down 된 마우스 포인터 위치</param>
	/// <returns> : 제대로 Spawn 되었다면 return true </returns>
	bool SpawnPingImage(FVector2D MousePos) override;

	// TODO : 이 함수 지우기
	bool SpawnJumpPosAndTargetPosImage(FVector JumpLocation, FVector TargetLocation);

protected:

	bool CancelPingMarker();


public:

	/// <summary>
	/// AirplaneLocation에 따른 AirplaneImagePosition 잡기
	/// </summary>
	/// <param name="AirplaneLocation"> : 비행기 현재 위치 </param>
	void UpdateAirplaneImagePosition(const FVector& AirplaneLocation);

	//void TogglePlayerMarkerImageVisibility(bool Visible);

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
	/// <summary>
	/// World 대비 Map size 척도로 적용된 위치 찾기
	/// </summary>
	/// <param name="GameWorldLocation"> : World Location </param>
	/// <returns> : 적용된 위치 FVector2D </returns>
	FVector2D GetWorldToMapSizePos(FVector GameWorldLocation) override;

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UImage* MainMapImg{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UImage* GridImg{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UBorder* PingMarkerBorder{};

protected: // TODO : 이 점들 지우기

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UImage* JumpPosImg{};
	FVector2D JumpPos{};
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UImage* TargetPosImg{};
	FVector2D TargetLocationPos{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UImage* EnemyLocationImg{};
	
private:

	float MainMapScaleLerpDest = 1.f;
	FVector2D MainMapPosLerpDest{};

	float MainMapScale = 1.f;

private:

	FVector2D PingMarkerPos{};

private:

	static const float		MAP_SCALE_MAX;
	static const float		SCROLL_DELTA_STEP;
	static const float		CANVAS_SIZE;
	static const FVector2D	MID_POINT;
	static const float		WORLD_MAP_SIZE;
	static const float		FIXED_LANDSCAPE_HEIGHT;
	static const float		PING_BORDER_SIZE;

private:

	bool bIsMouseDragging{};
	FVector2D PrevDragMousePos{};
	const float DRAG_SPEED = 5.f;


};


