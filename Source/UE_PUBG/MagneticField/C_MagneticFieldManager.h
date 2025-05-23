﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_MagneticFieldManager.generated.h"

UENUM(BlueprintType)
enum class EMagneticFieldState : uint8
{
	IDLE,
	SHRINK,
	SHRINK_COMPLETED
};

USTRUCT(BlueprintType)
struct FMagneticCircle
{
	GENERATED_BODY()

	void SetRadiusByMeter(const float& InMeter) { Radius = InMeter * 100.f; }

	FVector MidLocation{};
	float	Radius{};
};

USTRUCT(BlueprintType)
struct FPhaseInfo
{
	GENERATED_BODY()

	FPhaseInfo() {}

	FPhaseInfo(float _PhaseRadius, float _ShrinkDelayTime, float _ShrinkTotalTime, float _DamagePerSecond)
		:PhaseRadius(_PhaseRadius), ShrinkDelayTime(_ShrinkDelayTime), ShrinkTotalTime(_ShrinkTotalTime), DamagePerSecond(_DamagePerSecond) {}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float PhaseRadius{}; // 현재 페이즈의 Radius 크기

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ShrinkDelayTime{}; // 자기장 다음 목표 지점으로 줄어들기 시작하기 전까지의 Holding time
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ShrinkTotalTime{}; // 줄어드는 총 시간
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DamagePerSecond{}; // 초당 피해량

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bShouldTryToAvoidWater{}; // 물 쪽을 피해서 적용해야 하는 Phase 원인지
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 WaterTileCountLimit{}; // 물이 포함된 Tile의 개수 Limit -> bShouldTryAvoidWater가 true일 때 유효

public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bHasExactLocation{}; // 정확한 위치에 페이즈 원을 잡아야 하는지
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector ExactPhaseLocation{}; // bHasExactLocation이 true일 때 사용할 정확한 위치

public:
	/* 밑은 NextCircle 정보가 나왔을 때 계산해서 Setting 처리 */
	
	float RadiusShrinkSpeed{}; // 현재 Phase의 반지름 줄이는 속도값
	float MidPointMoveSpeed{}; // 현재 Phase의 중앙점 옮기는 속도값
	
	FVector MidPointMoveDirection{}; // 현재 Phase의 중앙점 옮기는 방향
};

UCLASS()
class UE_PUBG_API AC_MagneticFieldManager : public AActor
{
	GENERATED_BODY()
	
public:
	AC_MagneticFieldManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	/// <summary>
	/// MagneticField 페이즈 첫 시작 개시
	/// </summary>
	void ActivateMagneticField();

	/// <summary>
	/// 해당 캐릭터가 자기장 영역 안에 있는지 조사 
	/// </summary>
	bool IsInMainCircle(class AC_BasicCharacter* Character) const;

	FMagneticCircle GetMainCircle() const { return MainCircle; }

private:

	/// <summary>
	/// 자기장 FSM 핸들링
	/// </summary>
	void HandleUpdateState(const float& DeltaTime);

	/// <summary>
	/// MainCircle 밖에 있는 캐릭터들 Damage Handling 
	/// </summary>
	void HandleDamagingCharacters(const float& DeltaTime);

protected:

	/// <summary>
	/// 첫 자기장 Init, BluePrint BeginPlay에서 호출될 예정
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void InitManager();

	/// <summary>
	/// 블루프린트 내에서 호출할 함수
	/// </summary>
	UFUNCTION(BlueprintCallable)
	int32 GetSliceCount() const { return SLICE_COUNT; }

private:

	/// <summary>
	/// 벽면 오브젝트들 새로 Update
	/// </summary>
	/// <param name="MidLocation"> : Update시킬 원의 중앙 위치 </param>
	/// <param name="Radius"> : Update시킬 원의 반지름 </param>
	void UpdateWalls(const FVector& MidLocation, const float& Radius);

	/// <summary>
	/// Random한 다음 Next Circle 뽑기 & 줄어드는 속력 구해놓기 & 중앙점 옮기는 방향 구해놓기
	/// </summary>
	void SetRandomNextCircleAndSpeedDirection();

private:

	/// <summary>
	/// UI Map의 Main Circle 정보 업데이트
	/// </summary>
	void UpdateMainCircleInfoOnMapUI();

	/// <summary>
	/// UI Map의 Next Circle 정보 업데이트
	/// </summary>
	void UpdateNextCircleInfoOnMapUI();

private:
	
	/// <summary>
	/// 지연시간 1분, 30초, 10초, 5초별 HUD Notification 처리 
	/// </summary>
	void HandleDelayTimeHUDNotification();

	void InitDelayTimeLeftNotifiedChecker();
	
protected:

	// MainCircle을 In game 내에서 보여줄 오브젝트들
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<class AC_MagneticWall*> MagneticWalls{};

private:

	EMagneticFieldState MagneticFieldState{};

private:
	
	// 실질적인 자기장 영역 circle
	FMagneticCircle MainCircle{};

	// 다음 Phase로 줄어들 자기장 영역 circle 
	FMagneticCircle NextCircle{};

private:

	// 현재 자기장 phase
	int CurrentPhase = 1;

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<int, FPhaseInfo> PhaseInfos =
	{
		{1, FPhaseInfo(71300.f, 5.f, 20.f, 0.4f)},	// 713m -> 첫 페이즈의 MainCircle이 될 값
		{2, FPhaseInfo(40000.f, 5.f, 20.f, 0.7f)},	// 400m
		{3, FPhaseInfo(20000.f, 5.f, 20.f, 1.5f)},	// 200m
		{4, FPhaseInfo(10000.f, 5.f, 20.f, 4.f)},	// 100m
		{5, FPhaseInfo(5000.f, 5.f, 20.f, 7.f)},	// 50m
		{6, FPhaseInfo(0.f, 0.f, 0.f, 9.f)}			// 제일 마지막 도착 지점 (전체 Phase보다 하나 더 많게끔 만들어놔야 정상 작동함)
	};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<int, FPhaseInfo> TestPhaseInfos{};


	// 시간 재기용
	float Timer{};

private:

	const int	SLICE_COUNT		= 100;
	const int	LAST_PHASE		= PhaseInfos.Num() - 1;
	const float WALL_Z_LOCATION = 3500.f;
	const float MAP_LENGTH		= 100000.f;
	const float MAP_LENGTH_TO_UV_FACTOR = 0.00001f;

private:

	// 자기장이 줄어들고 다음 자기장 설정 등의 Managing이 실질적으로 시작되었는지 체크
	bool bIsHandleUpdateStateStarted{};

	class UC_WaterTileCheckerComponent* WaterTileCheckerComponent{};

private:

	// 남은 지연시간 별 Notify 처리해야할 지점 UI 띄워줬는지 체크하는 TMap 
	TMap<float, bool> DelayTimeLeftNotifiedChecker =
	{
		{60.f, false},
		{30.f, false},
		{10.f, false},
		{5.f,  false}
	};
	
};
