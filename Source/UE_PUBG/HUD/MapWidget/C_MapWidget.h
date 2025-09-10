// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Singleton/C_GameInstance.h"
#include "C_MapWidget.generated.h"

enum class ELevelType : uint8;
/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_MapWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;
	
protected:

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:

	void SetOwnerPlayer(class AC_Player* InOwnerPlayer) { OwnerPlayer = InOwnerPlayer; }

	bool GetIsPanelOpened() const { return this->GetVisibility() == ESlateVisibility::Visible || this->GetVisibility() == ESlateVisibility::SelfHitTestInvisible; }

public:
	
	/// <summary>
	/// Map 이미지의 MainCircle Material 관련 information 조정
	/// </summary>
	/// <param name="PosU"> : 중점의 U좌표 </param>
	/// <param name="PosV"> : 중점의 V좌표 </param>
	/// <param name="UVRad"> : 중점의 Radius(UV좌표계 단위) </param>
	void SetMainCircleInfo(float PosU, float PosV, float UVRad);

	/// <summary>
	/// Map 이미지의 NextCircle Material 관련 information 조정
	/// </summary>
	/// <param name="PosU"> : 중점의 U좌표 </param>
	/// <param name="PosV"> : 중점의 V좌표 </param>
	/// <param name="UVRad"> : 중점의 Radius(UV좌표계 단위) </param>
	void SetNextCircleInfo(float PosU, float PosV, float UVRad);

public:

	/// <summary>
	/// World에서 찍은 핑 마커 Spawn 처리 시키기
	/// </summary>
	/// <param name="WorldPingLocation"> : World에 찍힌 핑의 위치 </param>
	/// <returns> : Spawn이 제대로 이루어졌다면 return true </returns>
	virtual bool SpawnPingImage(FVector WorldPingLocation) PURE_VIRTUAL(UC_MapWidget::SpawnPingImage, return false; );

public: // Airplane 관련

	/// <summary>
	/// Start, Dest에 따른 비행기 경로 이미지 관련 위치 및 회전 잡기
	/// </summary>
	/// <param name="StartDest"> : 비행기 시작(낙하 가능 시작위치), 끝(낙하 가능 끝 위치) 위치</param>
	void SetAirplaneRoute(TPair<FVector, FVector> StartDest);

	/// <summary>
	/// Airplane Image Visibility Toggle 
	/// </summary>
	void ToggleAirplaneImageVisibility(bool Visible);

	/// <summary>
	/// 비행 경로 Visibility Toggle
	/// </summary>
	void ToggleAirplaneRouteVisibility(bool Visible);

	/// <summary>
	/// AirplaneLocation에 따른 AirplaneImagePosition 잡기
	/// </summary>
	/// <param name="AirplaneLocation"> : 비행기 현재 위치 </param>
	virtual void UpdateAirplaneImagePosition(const FVector& AirplaneLocation) PURE_VIRTUAL(UC_MapWidget::UpdateAirplaneImagePosition, );

private:

	/// <summary>
	/// 맵의 Transform 또는 Player의 위치에 따른 PlaneRoute 관련 Image Transform 조정
	/// </summary>
	virtual void HandleUpdatePlaneRouteTransform() PURE_VIRTUAL(UC_MapWidget::HandleUpdatePlaneRouteTransform, );

	/// <summary>
	/// World 대비 Map size 척도로 적용된 위치 찾기
	/// </summary>
	/// <param name="GameWorldLocation"> : World Location </param>
	/// <returns> : 적용된 위치 FVector2D </returns>
	virtual FVector2D GetWorldToMapSizePos(FVector GameWorldLocation) PURE_VIRTUAL(UC_MapWidget::GetWorldToMapSizePos, return FVector2D(); );

private:

	void UpdatePlayerMarkerRotation();

protected:

	AC_Player* OwnerPlayer{};

protected: // 비행기 경로 관련

	//FVector2D AirplaneRouteStartPosOrigin{};
	//FVector2D AirplaneRouteDestPosOrigin{};

	FVector2D AirplaneRouteStartPos{};
	FVector2D AirplaneRouteDestPos{};

protected:

	UPROPERTY(meta = (BindWidget))
	class UImage* MapImage{};

	UPROPERTY(meta = (BindWidget))
	UImage* GridImage{};
	
	UPROPERTY(meta = (BindWidget))
	UImage* PlayerMarker{};

	UPROPERTY(meta = (BindWidget))
	UImage* AirplaneStartCircle{};	

	UPROPERTY(meta = (BindWidget))
	UImage* AirplaneDestTriangle{};

	UPROPERTY(meta = (BindWidget))
	UImage* AirplaneImage{};

	UPROPERTY(meta = (BindWidget))
	UImage* AirplaneRouteLine{};
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UBorder* PingBorder{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSoftObjectPtr<UMaterialInstance> ShantyTownMapImgMaterialInstance{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> TrainingGroundMapImgTexture{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> ShantyTownMapGridImgTexture{};
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> TrainingGroundMapGridImgTexture{};
	
private:
	
	UMaterialInstanceDynamic* ShantyTownMapImgMaterialInstanceDynamic{};
	
protected:

	ELevelType CurrentSelectedMap{};

	FVector OriginLocation{};
	const TMap<ELevelType, FVector> OriginLocations =
	{
		{ELevelType::ShantyTown,		FVector(0.f)},	
		{ELevelType::TrainingGround,	FVector(5000.f, -11000.f, 0.f)}, 
		{ELevelType::QuickStart,		FVector(0.f)},	// Dummy
		{ELevelType::Max,				FVector(0.f)}	// Dummy
	};
	
protected:

	// AirplaneRouteImage의 size X를 조정하기 위함
	class UCanvasPanelSlot* AirplaneRouteImageCanvasSlot{};
	
};

