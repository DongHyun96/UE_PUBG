// Fill out your copyright notice in the Description page of Project Settings.


#include "Airplane/C_AirplaneManager.h"
#include "Airplane/C_Airplane.h"

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
	1. �� �߾����� �������� �ϴ� �� ���� ������ ���� ���
	2. �ش� ������ ���� ������ ���� Random�� ���� ���
	*/

	float XDir = FMath::FRandRange(-1.f, 1.f);
	float YDir = FMath::FRandRange(-1.f, 1.f);
	FVector RandomDirection = FVector(XDir, YDir, 0.f);
	RandomDirection.Normalize();

	const float CIRCLE_RADIUS = 20000.f; // 200m Radius
	float RandomScalar = FMath::RandRange(0.f, CIRCLE_RADIUS);
	FVector RandomPos = RandomDirection * RandomScalar; // �� ���� ������ ����

	// Random�� ���� ��� -> �� �����ڸ� ������ �ٸ� ���� ���������� ��� ������ ��ü
	float StartX{}, StartY{}, DestX{}, DestY{};
	const float PLANE_START_DEST_BORDER_VALUE = 40000.f;

	if (FMath::RandRange(0, 1) == 0)
	{
		StartX		= (FMath::RandRange(0, 1) == 0) ? PLANE_START_DEST_BORDER_VALUE : -PLANE_START_DEST_BORDER_VALUE;
		StartY		= FMath::FRandRange(-PLANE_START_DEST_BORDER_VALUE, PLANE_START_DEST_BORDER_VALUE);
		DestX		= -StartX;
		float Slope = (RandomPos.Y - StartY) / (RandomPos.X - StartX);
		DestY		= Slope * (DestX - StartX) + StartY;
	}
	else
	{
		StartY		= (FMath::RandRange(0, 1) == 0) ? PLANE_START_DEST_BORDER_VALUE : -PLANE_START_DEST_BORDER_VALUE;
		StartX		= FMath::FRandRange(-PLANE_START_DEST_BORDER_VALUE, PLANE_START_DEST_BORDER_VALUE);
		DestY		= -StartY;
		float Slope = (RandomPos.Y - StartY) / (RandomPos.X - StartX);
		DestX		= (DestY - StartY) / Slope + StartX;
	}

	PlaneRouteStart = FVector(StartX, StartY, 0.f);
	PlaneRouteDest  = FVector(DestX, DestY, 0.f);
}

