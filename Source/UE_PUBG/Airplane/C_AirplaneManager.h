// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_AirplaneManager.generated.h"

UCLASS()
class UE_PUBG_API AC_AirplaneManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AC_AirplaneManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	/// <summary>
	/// 비행기 비행 시작 시간 재기
	/// </summary>
	/// <param name="DeltaTime"></param>
	void UpdateTakeOffTimer(const float& DeltaTime);

public:

	TPair<FVector, FVector> GetPlaneRouteStartDestPair() const { return { PlaneRouteStart, PlaneRouteDest }; }
	class AC_Airplane* GetAirplane() const { return Airplane; }

	bool GetCanDive() const { return CanDive; }

	/// <summary>
	/// 비행 시작 전까지 몇 초 남았는지(TakeOffTimer) get 
	/// </summary>
	float GetTakeOffTimer() const { return TakeOffTimer; }

	bool GetHasAirplaneTakeOff() const { return HasAirplaneTakeOff; }

public:
	/// <summary>
	/// Random Start, Dest position 지정하기
	/// </summary>
	void InitRandomStartDestPosition();

private:

	/// <summary>
	/// 비행기 현재 위치에서 Dive할 수 있는지 CanDive 업데이트
	/// </summary>
	void UpdateCanDive();

	/// <summary>
	/// Start Dest pos 초기화 시 Border가 valid한지 체크
	/// </summary>
	/// <param name="PositionValue"></param>
	/// <returns></returns>
	bool IsValueValidInBorder(float PositionValue);

	/// <summary>
	/// 낙하해야 하는 Limit 거리까지 도달했는지 체크해서 도달했다면, 남아있는 캐릭터들 모두 SkyDiving 시키기
	/// </summary>
	void CheckAirplaneArrivedToRouteDestLimit();

	/// <summary>
	/// 비행기 도착 지점( Dest를 지나 맵 조금 밖)에 도착했는지 조사
	/// </summary>
	void CheckFlightFinished();

	/// <summary>
	/// Airplane의 StartPos와 FlightDirection Init
	/// </summary>
	void InitAirplaneStartPosAndFlightDirection();

private:

	void StartTakeOffTimer();

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class AC_Airplane* Airplane{};

private:

	// const float PLANE_ALTITUDE = 30000.f;
	const float PLANE_ALTITUDE = 60000.f; // 600m

	const float PLANE_START_DEST_BORDER_VALUE = 40000.f;

	// 비행기 경로 시작점, 끝점 (맵에 표시된 경로의 시작점, 끝점)
	FVector PlaneRouteStart{};
	FVector PlaneRouteDest{};

private:

	// 비행기 출발 시작 전까지의 Timer 시작했는지
	bool TakeOffTimerStarted{};

	// 비행기 출발 시작했는지
	bool HasAirplaneTakeOff{};

	float TakeOffTimer = 10.f; // 게임 시작까지 총 30초 (TakeOff 까지)

private:

	// 현재 비행기 위치에서 SkyDiving 할 수 있는지
	bool CanDive{};

	// 낙하 가능 지점 Limit에 끝까지 도달했는지
	bool RouteDestLimitReached{};

};


