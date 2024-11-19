// Fill out your copyright notice in the Description page of Project Settings.


#include "Airplane/C_AirplaneManager.h"
#include "Airplane/C_Airplane.h"
#include "Utility/C_Util.h"

// Sets default values
AC_AirplaneManager::AC_AirplaneManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AC_AirplaneManager::BeginPlay()
{
	Super::BeginPlay();

	InitRandomStartDestPosition();
	if (Airplane)
		Airplane->SetActorLocation(PlaneRouteDest);

}

// Called every frame
void AC_AirplaneManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AC_AirplaneManager::InitRandomStartDestPosition()
{
	/*
	1. 맵 중앙점을 기점으로 하는 원 안의 임의의 지점 잡기
	2. 해당 임의의 점을 지나는 선의 Random한 기울기 잡기
	*/

	float XDir				= FMath::FRandRange(-1.f, 1.f);
	float YDir				= FMath::FRandRange(-1.f, 1.f);
	FVector RandomDirection = FVector(XDir, YDir, 0.f);

	RandomDirection.Normalize();

	const float CIRCLE_RADIUS	= 20000.f; // 200m Radius
	float RandomScalar			= FMath::RandRange(0.f, CIRCLE_RADIUS);
	FVector RandomPos			= RandomDirection * RandomScalar; // 원 안의 임의의 지점

	// Random한 기울기 잡기 -> 맵 가장자리 임의의 다른 점을 시작점으로 잡는 것으로 대체
	float StartX{}, StartY{}, DestX{}, DestY{};
	float Slope{};

	if (FMath::RandRange(0, 1) == 0) // X값을 border에 맞출 때
	{
		StartX			= (FMath::RandRange(0, 1) == 0) ? PLANE_START_DEST_BORDER_VALUE : -PLANE_START_DEST_BORDER_VALUE;
		StartY			= FMath::FRandRange(-PLANE_START_DEST_BORDER_VALUE, PLANE_START_DEST_BORDER_VALUE);
		PlaneRouteStart = FVector(StartX, StartY, 0.f);

		// Y = Slope * X + YInterceptValue
		Slope = (RandomPos.Y - StartY) / (RandomPos.X - StartX);
		float YInterceptValue = StartY - Slope * StartX;

		if (Slope == 0.f)
		{
			PlaneRouteDest = FVector(-StartX, StartY, 0.f);
			return;
		}

		float DX{};
		float DY{};

		// X = +-PLANE_START_DEST_BORDER_VALUE 직선과 검사
		DX = -StartX;
		DY = Slope * DX + YInterceptValue;

		if (IsValueValidInBorder(DY))
		{
			PlaneRouteDest = FVector(DX, DY, 0.f);
			return;
		}

		// Y = +-PLANE_START_DEST_BORDER_VALUE 직선과 검사
		for (int i = -1; i <= 1; i += 2)
		{
			DY = PLANE_START_DEST_BORDER_VALUE * i;
			DX = (DY - YInterceptValue) / Slope;

			if (IsValueValidInBorder(DX))
			{
				PlaneRouteDest = FVector(DX, DY, 0.f);
				return;
			}
		}
	}
	else // Y값을 Border에 맞출 때
	{
		StartY			= (FMath::RandRange(0, 1) == 0) ? PLANE_START_DEST_BORDER_VALUE : -PLANE_START_DEST_BORDER_VALUE;
		StartX			= FMath::FRandRange(-PLANE_START_DEST_BORDER_VALUE, PLANE_START_DEST_BORDER_VALUE);
		PlaneRouteStart = FVector(StartX, StartY, 0.f);

		if (RandomPos.X - StartX == 0.f) // 기울기가 무한일 때
		{
			PlaneRouteDest = FVector(StartX, -StartY, 0.f);
			return;
		}

		// Y = Slope * X + YInterceptValue
		Slope = (RandomPos.Y - StartY) / (RandomPos.X - StartX);
		float YInterceptValue = StartY - Slope * StartX;

		float DX{};
		float DY{};

		// Y = +-PLANE_START_DEST_BORDER_VALUE 직선과 검사
		DY = -StartY;
		DX = (DY - YInterceptValue) / Slope;

		if (IsValueValidInBorder(DX))
		{
			PlaneRouteDest = FVector(DX, DY, 0.f);
			return;
		}

		// X = +-PLANE_START_DEST_BORDER_VALUE 직선과 검사
		for (int i = -1; i <= 1; i += 2)
		{
			DX = PLANE_START_DEST_BORDER_VALUE * i;
			DY = Slope * DX + YInterceptValue;

			if (IsValueValidInBorder(DY))
			{
				PlaneRouteDest = FVector(DX, DY, 0.f);
				return;
			}
		}
	}
}

bool AC_AirplaneManager::IsValueValidInBorder(float PositionValue)
{
	return FMath::Abs(PositionValue) <= PLANE_START_DEST_BORDER_VALUE;
}

