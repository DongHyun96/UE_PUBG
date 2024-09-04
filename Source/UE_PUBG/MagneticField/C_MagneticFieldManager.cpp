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

		if (Timer < PhaseInfos[CurrentPhase].ShrinkDelayTime) return; // �����ð��� ���� ������

		/* �����ð��� �� ������ �� */
		Timer = 0.f;

		MagneticFieldState = EMagneticFieldState::SHRINK;

		return;
	case EMagneticFieldState::SHRINK:


		// �پ��� �ð��� ������ ������ ��
		if (Timer > PhaseInfos[CurrentPhase].ShrinkTotalTime)
		{
			UpdateWalls(MainCircle.MidLocation, MainCircle.Radius);
			Timer = 0.f;
			MagneticFieldState = EMagneticFieldState::SHRINK_COMPLETED;
			return;
		}

		// TODO : ������ �ӷ� �� �������� MainCircle�� ���̰� �߾����� ������ �ӷ°� �������� �ű��
		MainCircle.Radius -= PhaseInfos[CurrentPhase].RadiusShrinkSpeed * DeltaTime;

		MainCircle.MidLocation +=	PhaseInfos[CurrentPhase].MidPointMoveDirection * 
									PhaseInfos[CurrentPhase].MidPointMoveSpeed *
									DeltaTime;

		UpdateWalls(MainCircle.MidLocation, MainCircle.Radius);

		return;
	case EMagneticFieldState::SHRINK_COMPLETED:
	{
		// �� ��Ȯ�� �����ֱ�
		MainCircle = NextCircle;

		// ������ phase�� Shrink completed
		if (CurrentPhase >= LAST_PHASE) return;

		CurrentPhase++;

		// ������ �ڱ����� �پ��� ���� �� ���
		if (CurrentPhase == LAST_PHASE)
		{
			NextCircle.MidLocation = MainCircle.MidLocation;
			NextCircle.Radius = PhaseInfos[CurrentPhase + 1].PhaseRadius;

			// �ӷ� ����
			float RadiusDifference = MainCircle.Radius - NextCircle.Radius;
			PhaseInfos[CurrentPhase].RadiusShrinkSpeed = RadiusDifference / PhaseInfos[CurrentPhase].ShrinkTotalTime;
			PhaseInfos[CurrentPhase].MidPointMoveSpeed = 0.f;
		}
		else SetRandomNextCircleAndSpeedDirection(); // Random�� Next Circle �̱�
			
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
	// ù Main �ڱ����� �ƿ� �� �ۿ� ���� -> 1������ �پ�� ���� NextCircle�� �پ����� �� ��μ� �ʿ� ����

	// ��������� �ʱ�ȭ
	CurrentPhase		= 1;
	MagneticFieldState	= EMagneticFieldState::IDLE;
	Timer				= 0.f;
	
	MainCircle.MidLocation	= { 0.f, 0.f, 0.f }; // TODO : ���� �������� setting
	MainCircle.Radius		= PhaseInfos[1].PhaseRadius;

	// Next Circle �������ֱ�
	SetRandomNextCircleAndSpeedDirection();

	UpdateWalls(MainCircle.MidLocation, MainCircle.Radius);
	TestUpdateWalls(NextCircle.MidLocation, NextCircle.Radius);
}

void AC_MagneticFieldManager::UpdateWalls(const FVector& MidLocation, const float& Radius)
{
	FVector FirstPos = FVector(MidLocation.X + Radius, MidLocation.Y, MidLocation.Z);
	MagneticWalls[0]->SetActorLocation(FirstPos);

	float WallWidth = 2 * PI * Radius / float(SLICE_COUNT);
	WallWidth *= 0.01f; // �⺻ ������ 1m�� ������ ����

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
	WallWidth *= 0.01f; // �⺻ ������ 1m�� ������ ����

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

	// �ӷ� setting
	float RadiusDifference = MainCircle.Radius - NextCircle.Radius;
	PhaseInfos[CurrentPhase].RadiusShrinkSpeed = RadiusDifference / PhaseInfos[CurrentPhase].ShrinkTotalTime;

	float MidPointsDistance = FVector::Distance(MainCircle.MidLocation, NextCircle.MidLocation);
	PhaseInfos[CurrentPhase].MidPointMoveSpeed = MidPointsDistance / PhaseInfos[CurrentPhase].ShrinkTotalTime;

	// Direction setting
	PhaseInfos[CurrentPhase].MidPointMoveDirection = NextCircle.MidLocation - MainCircle.MidLocation;
	PhaseInfos[CurrentPhase].MidPointMoveDirection.Normalize();
}


