// Fill out your copyright notice in the Description page of Project Settings.


#include "MagneticField/C_MagneticFieldManager.h"
#include "C_MagneticWall.h"
#include "UObject/ConstructorHelpers.h"
#include "Utility/C_Util.h"


AC_MagneticFieldManager::AC_MagneticFieldManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_MagneticFieldManager::BeginPlay()
{
	Super::BeginPlay();

}

void AC_MagneticFieldManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Timer += DeltaTime;

	//UC_Util::Print(CurrentPhase);

	HandleUpdateState(DeltaTime);
}

void AC_MagneticFieldManager::HandleUpdateState(const float& DeltaTime)
{
	switch (MagneticFieldState)
	{
	case EMagneticFieldState::IDLE:

		if (Timer < PhaseInfos[CurrentPhase].ShrinkDelayTime) return; // 보류시간이 아직 남았음

		/* 보류시간이 다 지났을 때 */
		Timer = 0.f;

		MagneticFieldState = EMagneticFieldState::SHRINK;

		return;
	case EMagneticFieldState::SHRINK:


		// 줄어드는 시간이 완전히 끝났을 때
		if (Timer > PhaseInfos[CurrentPhase].ShrinkTotalTime)
		{
			UpdateWalls(MainCircle.MidLocation, MainCircle.Radius);
			Timer = 0.f;
			MagneticFieldState = EMagneticFieldState::SHRINK_COMPLETED;
			return;
		}

		// TODO : 일정한 속력 및 방향으로 MainCircle을 줄이고 중앙점도 일정한 속력과 방향으로 옮기기
		MainCircle.Radius -= PhaseInfos[CurrentPhase].RadiusShrinkSpeed * DeltaTime;

		MainCircle.MidLocation +=	PhaseInfos[CurrentPhase].MidPointMoveDirection * 
									PhaseInfos[CurrentPhase].MidPointMoveSpeed *
									DeltaTime;

		UpdateWalls(MainCircle.MidLocation, MainCircle.Radius);

		return;
	case EMagneticFieldState::SHRINK_COMPLETED:
	{
		// 값 정확히 맞춰주기
		MainCircle = NextCircle;

		// 마지막 phase의 Shrink completed
		if (CurrentPhase >= LAST_PHASE) return;

		CurrentPhase++;

		// 마지막 자기장의 줄어드는 곳은 정 가운데
		if (CurrentPhase == LAST_PHASE)
		{
			NextCircle.MidLocation = MainCircle.MidLocation;
			NextCircle.Radius = PhaseInfos[CurrentPhase + 1].PhaseRadius;

			// 속력 세팅
			float RadiusDifference = MainCircle.Radius - NextCircle.Radius;
			PhaseInfos[CurrentPhase].RadiusShrinkSpeed = RadiusDifference / PhaseInfos[CurrentPhase].ShrinkTotalTime;
			PhaseInfos[CurrentPhase].MidPointMoveSpeed = 0.f;
		}
		else SetRandomNextCircleAndSpeedDirection(); // Random한 Next Circle 뽑기
			
		TestUpdateWalls(NextCircle.MidLocation, NextCircle.Radius);

		MagneticFieldState = EMagneticFieldState::IDLE;

		Timer = 0.f;

	}
		return;
	default: 
		return;
	}
}

void AC_MagneticFieldManager::InitManager()
{
	// 첫 Main 자기장은 아예 맵 밖에 존재 -> 1페이즈 줄어들 때의 NextCircle로 줄어들었을 때 비로소 맵에 잡힘

	// 멤버변수들 초기화
	CurrentPhase		= 1;
	MagneticFieldState	= EMagneticFieldState::IDLE;
	Timer				= 0.f;
	
	MainCircle.MidLocation	= { 0.f, 0.f, 0.f }; // TODO : 맵의 중점으로 setting
	MainCircle.Radius		= PhaseInfos[1].PhaseRadius;

	// Next Circle 설정해주기
	SetRandomNextCircleAndSpeedDirection();

	UpdateWalls(MainCircle.MidLocation, MainCircle.Radius);
	TestUpdateWalls(NextCircle.MidLocation, NextCircle.Radius);
}

void AC_MagneticFieldManager::UpdateWalls(const FVector& MidLocation, const float& Radius)
{
	FVector FirstPos = FVector(MidLocation.X + Radius, MidLocation.Y, MidLocation.Z);
	MagneticWalls[0]->SetActorLocation(FirstPos);

	float WallWidth = 2 * PI * Radius / float(SLICE_COUNT);
	WallWidth *= 0.01f; // 기본 단위가 1m로 맞춰져 있음

	MagneticWalls[0]->SetActorScale3D(FVector(100.f, WallWidth, 1.f));
	MagneticWalls[0]->UpdateMaterialVTiling();

	for (int i = 1; i < SLICE_COUNT; i++)
	{
		MagneticWalls[i]->SetActorScale3D(FVector(100.f, WallWidth, 1.f));

		float DegTheta = 360.f * i / SLICE_COUNT;
		MagneticWalls[i]->SetActorRotation(FRotator(90.f, DegTheta, 0.f));

		float XPos = Radius * FMath::Cos(2 * PI * i / SLICE_COUNT);
		float YPos = Radius * FMath::Sin(2 * PI * i / SLICE_COUNT);

		FVector CurrentPos = FVector(MidLocation.X + XPos, MidLocation.Y + YPos, MidLocation.Z);

		MagneticWalls[i]->SetActorLocation(CurrentPos);
		MagneticWalls[i]->UpdateMaterialVTiling();
	}
}

void AC_MagneticFieldManager::TestUpdateWalls(const FVector& MidLocation, const float& Radius)
{
	FVector FirstPos = FVector(MidLocation.X + Radius, MidLocation.Y, MidLocation.Z);
	ManeticWallsTemp[0]->SetActorLocation(FirstPos);

	float WallWidth = 2 * PI * Radius / float(SLICE_COUNT);
	WallWidth *= 0.01f; // 기본 단위가 1m로 맞춰져 있음

	ManeticWallsTemp[0]->SetActorScale3D(FVector(100.f, WallWidth, 1.f));

	for (int i = 1; i < SLICE_COUNT; i++)
	{
		ManeticWallsTemp[i]->SetActorScale3D(FVector(100.f, WallWidth, 1.f));

		float DegTheta = 360.f * i / SLICE_COUNT;
		ManeticWallsTemp[i]->SetActorRotation(FRotator(90.f, DegTheta, 0.f));

		float XPos = Radius * FMath::Cos(2 * PI * i / SLICE_COUNT);
		float YPos = Radius * FMath::Sin(2 * PI * i / SLICE_COUNT);

		FVector CurrentPos = FVector(MidLocation.X + XPos, MidLocation.Y + YPos, MidLocation.Z);

		ManeticWallsTemp[i]->SetActorLocation(CurrentPos);
	}
}

void AC_MagneticFieldManager::SetRandomNextCircleAndSpeedDirection()
{
	// Next random circle setting

	float XDir = FMath::FRandRange(-1.f, 1.f);
	float YDir = FMath::FRandRange(-1.f, 1.f);

	FVector RandomDirection = FVector(XDir, YDir, 0.f);
	RandomDirection.Normalize();

	float RandomScalar = FMath::FRandRange(0.f, MainCircle.Radius - PhaseInfos[CurrentPhase + 1].PhaseRadius);

	FVector RandomNextCirclePos = MainCircle.MidLocation + RandomDirection * RandomScalar;

	NextCircle.Radius		= PhaseInfos[CurrentPhase + 1].PhaseRadius;
	NextCircle.MidLocation	= RandomNextCirclePos;

	// 속력 setting
	float RadiusDifference = MainCircle.Radius - NextCircle.Radius;
	PhaseInfos[CurrentPhase].RadiusShrinkSpeed = RadiusDifference / PhaseInfos[CurrentPhase].ShrinkTotalTime;

	float MidPointsDistance = FVector::Distance(MainCircle.MidLocation, NextCircle.MidLocation);
	PhaseInfos[CurrentPhase].MidPointMoveSpeed = MidPointsDistance / PhaseInfos[CurrentPhase].ShrinkTotalTime;

	// Direction setting
	PhaseInfos[CurrentPhase].MidPointMoveDirection = NextCircle.MidLocation - MainCircle.MidLocation;
	PhaseInfos[CurrentPhase].MidPointMoveDirection.Normalize();
}


