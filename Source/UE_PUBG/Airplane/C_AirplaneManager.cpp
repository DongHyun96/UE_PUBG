// Fill out your copyright notice in the Description page of Project Settings.


#include "Airplane/C_AirplaneManager.h"
#include "Airplane/C_Airplane.h"
#include "Utility/C_Util.h"

#include "Singleton/C_GameSceneManager.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Character/Component/C_SkyDivingComponent.h"

#include "HUD/C_HUDWidget.h"
#include "HUD/C_MainMapWidget.h"
#include "HUD/C_InstructionWidget.h"

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
	if (Airplane) InitAirplaneStartPosAndFlightDirection();

	// Airplane TakeOff Timer Setting
	//GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AC_AirplaneManager::StartTakeOffTimer, 5.f, false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AC_AirplaneManager::StartTakeOffTimer, 0.5f, false);

}

// Called every frame
void AC_AirplaneManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateTakeOffTimer(DeltaTime);

	UpdateCanDive();
	CheckAirplaneArrivedToRouteDestLimit();
	CheckFlightFinished();
}

void AC_AirplaneManager::UpdateTakeOffTimer(const float& DeltaTime)
{
	if (!TakeOffTimerStarted) return;
	if (HasAirplaneTakeOff) return;

	TakeOffTimer -= DeltaTime;

	if (TakeOffTimer <= 0.f) // �̷� ����
	{
		InitAirplaneStartPosAndFlightDirection();
		Airplane->StartFlight();
		HasAirplaneTakeOff = true;

		GAMESCENE_MANAGER->GetAllCharacters();
		for (AC_BasicCharacter* Character : GAMESCENE_MANAGER->GetAllCharacters())
		{
			Character->SetMainState(EMainState::SKYDIVING);
			Character->GetSkyDivingComponent()->SetSkyDivingState(ESkyDivingState::READY);
		}
	}
}

void AC_AirplaneManager::InitRandomStartDestPosition()
{
	/*
	1. �� �߾����� �������� �ϴ� �� ���� ������ ���� ���
	2. �ش� ������ ���� ������ ���� Random�� ���� ���
	*/

	float XDir				= FMath::FRandRange(-1.f, 1.f);
	float YDir				= FMath::FRandRange(-1.f, 1.f);
	FVector RandomDirection = FVector(XDir, YDir, 0.f);

	RandomDirection.Normalize();

	const float CIRCLE_RADIUS	= 20000.f; // 200m Radius
	float RandomScalar			= FMath::RandRange(0.f, CIRCLE_RADIUS);
	FVector RandomPos			= RandomDirection * RandomScalar; // �� ���� ������ ����

	// Random�� ���� ��� -> �� �����ڸ� ������ �ٸ� ���� ���������� ��� ������ ��ü
	float StartX{}, StartY{}, DestX{}, DestY{};
	float Slope{};

	if (FMath::RandRange(0, 1) == 0) // X���� border�� ���� ��
	{
		StartX			= (FMath::RandRange(0, 1) == 0) ? PLANE_START_DEST_BORDER_VALUE : -PLANE_START_DEST_BORDER_VALUE;
		StartY			= FMath::FRandRange(-PLANE_START_DEST_BORDER_VALUE, PLANE_START_DEST_BORDER_VALUE);
		PlaneRouteStart = FVector(StartX, StartY, PLANE_ALTITUDE);

		// Y = Slope * X + YInterceptValue
		Slope = (RandomPos.Y - StartY) / (RandomPos.X - StartX);
		float YInterceptValue = StartY - Slope * StartX;

		if (Slope == 0.f)
		{
			PlaneRouteDest = FVector(-StartX, StartY, PLANE_ALTITUDE);
			return;
		}

		float DX{};
		float DY{};

		// X = +-PLANE_START_DEST_BORDER_VALUE ������ �˻�
		DX = -StartX;
		DY = Slope * DX + YInterceptValue;

		if (IsValueValidInBorder(DY))
		{
			PlaneRouteDest = FVector(DX, DY, PLANE_ALTITUDE);
			return;
		}

		// Y = +-PLANE_START_DEST_BORDER_VALUE ������ �˻�
		for (int i = -1; i <= 1; i += 2)
		{
			DY = PLANE_START_DEST_BORDER_VALUE * i;
			DX = (DY - YInterceptValue) / Slope;

			if (IsValueValidInBorder(DX))
			{
				PlaneRouteDest = FVector(DX, DY, PLANE_ALTITUDE);
				return;
			}
		}
	}
	else // Y���� Border�� ���� ��
	{
		StartY			= (FMath::RandRange(0, 1) == 0) ? PLANE_START_DEST_BORDER_VALUE : -PLANE_START_DEST_BORDER_VALUE;
		StartX			= FMath::FRandRange(-PLANE_START_DEST_BORDER_VALUE, PLANE_START_DEST_BORDER_VALUE);
		PlaneRouteStart = FVector(StartX, StartY, PLANE_ALTITUDE);

		if (RandomPos.X - StartX == 0.f) // ���Ⱑ ������ ��
		{
			PlaneRouteDest = FVector(StartX, -StartY, PLANE_ALTITUDE);
			return;
		}

		// Y = Slope * X + YInterceptValue
		Slope = (RandomPos.Y - StartY) / (RandomPos.X - StartX);
		float YInterceptValue = StartY - Slope * StartX;

		float DX{};
		float DY{};

		// Y = +-PLANE_START_DEST_BORDER_VALUE ������ �˻�
		DY = -StartY;
		DX = (DY - YInterceptValue) / Slope;

		if (IsValueValidInBorder(DX))
		{
			PlaneRouteDest = FVector(DX, DY, PLANE_ALTITUDE);
			return;
		}

		// X = +-PLANE_START_DEST_BORDER_VALUE ������ �˻�
		for (int i = -1; i <= 1; i += 2)
		{
			DX = PLANE_START_DEST_BORDER_VALUE * i;
			DY = Slope * DX + YInterceptValue;

			if (IsValueValidInBorder(DY))
			{
				PlaneRouteDest = FVector(DX, DY, PLANE_ALTITUDE);
				return;
			}
		}
	}
}

void AC_AirplaneManager::UpdateCanDive()
{
	// CanDive�� �� ���� �Ѿ��� �� true�� üũ�ϸ� ��
	if (!HasAirplaneTakeOff || !Airplane->GetIsFlying() || CanDive) return;
	
	FVector FlightDirection = PlaneRouteDest - PlaneRouteStart;
	FlightDirection.Normalize();

	FVector AirplaneLocation = Airplane->GetActorLocation();
	FVector StartToAirplaneDirection = AirplaneLocation - PlaneRouteStart;
	StartToAirplaneDirection.Normalize();

	CanDive = FlightDirection.Equals(StartToAirplaneDirection, KINDA_SMALL_NUMBER);

	// Player HUD ������Ʈ
	if (CanDive) GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetInstructionWidget()->ToggleEjectInstructionVisibility(true);
}

bool AC_AirplaneManager::IsValueValidInBorder(float PositionValue)
{
	return FMath::Abs(PositionValue) <= PLANE_START_DEST_BORDER_VALUE;
}

void AC_AirplaneManager::CheckAirplaneArrivedToRouteDestLimit()
{
	// �������� Limit�� �Ѿ�ٸ� ���� ������ ���� ĳ���͵� ������ ó��
	if (!Airplane->GetIsFlying()) return;

	FVector FlightDirection = PlaneRouteDest - PlaneRouteStart;
	FlightDirection.Normalize();

	FVector AirplaneLocation = Airplane->GetActorLocation();
	FVector DestToAirplaneDirection = AirplaneLocation - PlaneRouteDest;
	DestToAirplaneDirection.Normalize();

	// ���� ���� Limit�� �Ѿ ����
	if (FlightDirection.Equals(DestToAirplaneDirection, KINDA_SMALL_NUMBER))
	{
		RouteDestLimitReached = true;

		for (AC_BasicCharacter* Character : GAMESCENE_MANAGER->GetAllCharacters())
		{
			if (Character->GetSkyDivingComponent()->GetSkyDivingState() == ESkyDivingState::READY)
				Character->GetSkyDivingComponent()->SetSkyDivingState(ESkyDivingState::SKYDIVING);
		}
	}
}

void AC_AirplaneManager::CheckFlightFinished()
{
	if (!HasAirplaneTakeOff || !Airplane->GetIsFlying()) return;
	if (!RouteDestLimitReached) return;

	// ���� �������� üũ (�� �ݰ濡 �ƿ� �������� üũ)
	// �����ٸ� -> ����� IsFlying false | MainMap UI Toggle airplane visibility
	FVector AirplaneLocation = Airplane->GetActorLocation();

	const float ACTUAL_START_DEST_BORDER_VALUE = 50000.f;

	if (FMath::Abs(AirplaneLocation.X) > ACTUAL_START_DEST_BORDER_VALUE ||
		FMath::Abs(AirplaneLocation.Y) > ACTUAL_START_DEST_BORDER_VALUE)
	{
		Airplane->SetIsFlying(false);
		UC_HUDWidget* HUDWidget = GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget();
		HUDWidget->GetMainMapWidget()->ToggleAirplaneImageVisibility(false);
		HUDWidget->GetMiniMapWidget()->ToggleAirplaneImageVisibility(false);
	}
}

void AC_AirplaneManager::InitAirplaneStartPosAndFlightDirection()
{
	// TODO : Init Airplane actual StartPosition and FlightDirection
	// L(t) = (x0 + t dx, y0 + t dy)

	FVector FlightDirection = PlaneRouteDest - PlaneRouteStart;
	FlightDirection.Normalize();
	Airplane->SetFlightDirection(FlightDirection);

	FVector ToStartDirection = -FlightDirection;

	const float ACTUAL_START_DEST_BORDER_VALUE = 50000.f;
	// x = +-START_DEST_BORDER_VALUE | y = +-START_DEST_BORDER_VALUE ������ ã�Ƽ� Valid�� �������� ã��

	// x = +-START_DEST_BORDER_VALUE ������ ã��
	if (!FMath::IsNearlyZero(ToStartDirection.X)) // ���μ��� �������� ���� ��
	{
		for (int i = -1; i <= 1; i += 2)
		{
			float X = ACTUAL_START_DEST_BORDER_VALUE * i;
			float T = (X - PlaneRouteStart.X) / ToStartDirection.X;
			float Y = PlaneRouteStart.Y + T * ToStartDirection.Y;

			if (T < 0.f) continue;

			// Check Border validation
			if (FMath::Abs(Y) <= ACTUAL_START_DEST_BORDER_VALUE)
			{
				Airplane->SetStartPosition(FVector(X, Y, PLANE_ALTITUDE));
				return;
			}
		}
	}

	if (!FMath::IsNearlyZero(ToStartDirection.Y)) // ���μ��� �������� ���� ��
	{
		for (int i = -1; i <= 1; i += 2)
		{
			float Y = ACTUAL_START_DEST_BORDER_VALUE * i;
			float T = (Y - PlaneRouteStart.Y) / ToStartDirection.Y;
			float X = PlaneRouteStart.X + T * ToStartDirection.X;

			if (T < 0.f) continue;

			// Check Border validation
			if (FMath::Abs(Y) <= ACTUAL_START_DEST_BORDER_VALUE)
			{
				Airplane->SetStartPosition(FVector(X, Y, PLANE_ALTITUDE));
				return;
			}
		}
	}

}

