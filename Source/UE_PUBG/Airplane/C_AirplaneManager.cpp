// Fill out your copyright notice in the Description page of Project Settings.


#include "Airplane/C_AirplaneManager.h"

#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"
#include "Airplane/C_Airplane.h"
#include "Utility/C_Util.h"

#include "Singleton/C_GameSceneManager.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
#include "Character/Component/C_SkyDivingComponent.h"

#include "HUD/C_HUDWidget.h"
#include "HUD/C_MainMapWidget.h"
#include "HUD/C_InstructionWidget.h"
#include "MagneticField/C_MagneticFieldManager.h"

AC_AirplaneManager::AC_AirplaneManager()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AC_AirplaneManager::BeginPlay()
{
	Super::BeginPlay();
	
	InitRandomStartDestPosition();
	
	// Airplane TakeOff Timer Setting
	// GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AC_AirplaneManager::StartTakeOffTimer, 10.f, false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AC_AirplaneManager::StartTakeOffTimer, 0.5f, false);

	if (!IsValid(Airplane))
	{
		UC_Util::Print("From AC_AirplaneManager::BeginPlay : Airplane not valid!", FColor::MakeRandomColor(), 10.f);
		return;
	}

	InitAirplaneStartPosAndFlightDirection();
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

	if (TakeOffTimer <= 0.f) // 이륙 시작
	{
		Airplane->StartFlight();
		HasAirplaneTakeOff = true;

		//GAMESCENE_MANAGER->GetPlayer()->SetMainState(EMainState::SKYDIVING);
		//GAMESCENE_MANAGER->GetPlayer()->GetSkyDivingComponent()->SetSkyDivingState(ESkyDivingState::READY);
		for (AC_BasicCharacter* Character : GAMESCENE_MANAGER->GetAllCharacters())
		{
			Character->SetMainState(EMainState::SKYDIVING);
			Character->GetSkyDivingComponent()->SetSkyDivingState(ESkyDivingState::READY);

			if (AC_Enemy* Enemy = Cast<AC_Enemy>(Character))
			{
				UC_BehaviorComponent* EnemyBehvaiorComponent = Enemy->GetEnemyAIController()->GetBehaviorComponent(); 
				EnemyBehvaiorComponent->SetServiceType(EServiceType::SKYDIVE);
				// EnemyBehvaiorComponent->SetIdleTaskType;
			}
		}
	}
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

		// X = +-PLANE_START_DEST_BORDER_VALUE 직선과 검사
		DX = -StartX;
		DY = Slope * DX + YInterceptValue;

		if (IsValueValidInBorder(DY))
		{
			PlaneRouteDest = FVector(DX, DY, PLANE_ALTITUDE);
			return;
		}

		// Y = +-PLANE_START_DEST_BORDER_VALUE 직선과 검사
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
	else // Y값을 Border에 맞출 때
	{
		StartY			= (FMath::RandRange(0, 1) == 0) ? PLANE_START_DEST_BORDER_VALUE : -PLANE_START_DEST_BORDER_VALUE;
		StartX			= FMath::FRandRange(-PLANE_START_DEST_BORDER_VALUE, PLANE_START_DEST_BORDER_VALUE);
		PlaneRouteStart = FVector(StartX, StartY, PLANE_ALTITUDE);

		if (RandomPos.X - StartX == 0.f) // 기울기가 무한일 때
		{
			PlaneRouteDest = FVector(StartX, -StartY, PLANE_ALTITUDE);
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
			PlaneRouteDest = FVector(DX, DY, PLANE_ALTITUDE);
			return;
		}

		// X = +-PLANE_START_DEST_BORDER_VALUE 직선과 검사
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
	// CanDive는 한 번만 넘었을 때 true로 체크하면 됨
	if (!HasAirplaneTakeOff || !Airplane->GetIsFlying() || CanDive) return;
	
	FVector FlightDirection = PlaneRouteDest - PlaneRouteStart;
	FlightDirection.Normalize();

	FVector AirplaneLocation = Airplane->GetActorLocation();
	FVector StartToAirplaneDirection = AirplaneLocation - PlaneRouteStart;
	StartToAirplaneDirection.Normalize();

	CanDive = FlightDirection.Equals(StartToAirplaneDirection, KINDA_SMALL_NUMBER);

	// Player HUD 업데이트
	if (CanDive) GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetInstructionWidget()->ToggleEjectInstructionVisibility(true);
}

bool AC_AirplaneManager::IsValueValidInBorder(float PositionValue)
{
	return FMath::Abs(PositionValue) <= PLANE_START_DEST_BORDER_VALUE;
}

void AC_AirplaneManager::CheckAirplaneArrivedToRouteDestLimit()
{
	// 낙하지점 Limit을 넘어갔다면 아직 내리지 않은 캐릭터들 내리게 처리
	if (!Airplane->GetIsFlying()) return;

	FVector FlightDirection = PlaneRouteDest - PlaneRouteStart;
	FlightDirection.Normalize();

	FVector AirplaneLocation = Airplane->GetActorLocation();
	FVector DestToAirplaneDirection = AirplaneLocation - PlaneRouteDest;
	DestToAirplaneDirection.Normalize();

	// 낙하 지점 Limit을 넘어간 시점
	if (FlightDirection.Equals(DestToAirplaneDirection, KINDA_SMALL_NUMBER))
	{
		RouteDestLimitReached = true;

		// 자기장 Phase 시작
		GAMESCENE_MANAGER->GetMagneticFieldManager()->SetIsHandleUpdateStateStarted(true);

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

	// 비행 끝났는지 체크 (맵 반경에 아예 나갔는지 체크)
	// 끝났다면 -> 비행기 IsFlying false | MainMap UI Toggle airplane visibility
	FVector AirplaneLocation = Airplane->GetActorLocation();

	const float ACTUAL_START_DEST_BORDER_VALUE = 50000.f;

	if (FMath::Abs(AirplaneLocation.X) > ACTUAL_START_DEST_BORDER_VALUE ||
		FMath::Abs(AirplaneLocation.Y) > ACTUAL_START_DEST_BORDER_VALUE)
	{
		Airplane->SetIsFlying(false);
		AC_Player* Player = GAMESCENE_MANAGER->GetPlayer();
		Player->GetMainMapWidget()->ToggleAirplaneImageVisibility(false);
		Player->GetHUDWidget()->GetMiniMapWidget()->ToggleAirplaneImageVisibility(false);
	}
}

void AC_AirplaneManager::InitAirplaneStartPosAndFlightDirection()
{
	/* Init Airplane actual StartPosition and FlightDirection */
	// L(t) = (x0 + t dx, y0 + t dy)

	FVector FlightDirection = PlaneRouteDest - PlaneRouteStart;
	FlightDirection.Normalize();
	Airplane->SetFlightDirection(FlightDirection);

	FVector ToStartDirection = -FlightDirection;

	const float ACTUAL_START_DEST_BORDER_VALUE = 50000.f;
	// x = +-START_DEST_BORDER_VALUE | y = +-START_DEST_BORDER_VALUE 교차점 찾아서 Valid한 교차지점 찾기

	// x = +-START_DEST_BORDER_VALUE 교차점 찾기
	if (!FMath::IsNearlyZero(ToStartDirection.X)) // 세로선과 평행이지 않을 때
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

	if (!FMath::IsNearlyZero(ToStartDirection.Y)) // 가로선과 평행이지 않을 때
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



