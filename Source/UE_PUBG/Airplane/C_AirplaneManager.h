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
	/// ����� ���� ���� �ð� ���
	/// </summary>
	/// <param name="DeltaTime"></param>
	void UpdateTakeOffTimer(const float& DeltaTime);

public:

	TPair<FVector, FVector> GetPlaneRouteStartDestPair() const { return { PlaneRouteStart, PlaneRouteDest }; }
	class AC_Airplane* GetAirplane() const { return Airplane; }

	bool GetCanDive() const { return CanDive; }


public:
	/// <summary>
	/// Random Start, Dest position �����ϱ�
	/// </summary>
	void InitRandomStartDestPosition();

private:

	/// <summary>
	/// ����� ���� ��ġ���� Dive�� �� �ִ��� CanDive ������Ʈ
	/// </summary>
	void UpdateCanDive();

	/// <summary>
	/// Start Dest pos �ʱ�ȭ �� Border�� valid���� üũ
	/// </summary>
	/// <param name="PositionValue"></param>
	/// <returns></returns>
	bool IsValueValidInBorder(float PositionValue);

	/// <summary>
	/// �����ؾ� �ϴ� Limit �Ÿ����� �����ߴ��� üũ�ؼ� �����ߴٸ�, �����ִ� ĳ���͵� ��� SkyDiving ��Ű��
	/// </summary>
	void CheckAirplaneArrivedToRouteDestLimit();

	/// <summary>
	/// ����� ���� ����( Dest�� ���� �� ���� ��)�� �����ߴ��� ����
	/// </summary>
	void CheckFlightFinished();

	/// <summary>
	/// Airplane�� StartPos�� FlightDirection Init
	/// </summary>
	void InitAirplaneStartPosAndFlightDirection();

private:

	void StartTakeOffTimer() { TakeOffTimerStarted = true; }

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class AC_Airplane* Airplane{};

private:

	const float PLANE_ALTITUDE = 30000.f; // 600m
	//const float PLANE_ALTITUDE = 60000.f; // 600m

	const float PLANE_START_DEST_BORDER_VALUE = 40000.f;

	// ����� ��� ������, ���� (�ʿ� ǥ�õ� ����� ������, ����)
	FVector PlaneRouteStart{};
	FVector PlaneRouteDest{};

private:

	struct FTimerHandle TimerHandle{};

	// ����� ��� ���� �������� Timer �����ߴ���
	bool TakeOffTimerStarted{};

	// ����� ��� �����ߴ���
	bool HasAirplaneTakeOff{};

	//const float TAKEOFF_TIME_TOTAL = 30.f;
	const float TAKEOFF_TIME_TOTAL = 1.f;
	float		TakeOffTimer = TAKEOFF_TIME_TOTAL;

private:

	// ���� ����� ��ġ���� SkyDiving �� �� �ִ���
	bool CanDive{};

	// ���� ���� ���� Limit�� ������ �����ߴ���
	bool RouteDestLimitReached{};

};
