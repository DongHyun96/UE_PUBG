// Fill out your copyright notice in the Description page of Project Settings.

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

	FPhaseInfo(float _PhaseRadius, float _ShrinkDelayTime, float _ShrinkTotalTime)
		:PhaseRadius(_PhaseRadius), ShrinkDelayTime(_ShrinkDelayTime), ShrinkTotalTime(_ShrinkTotalTime) {}
	
	// ���� �������� Radius ũ��
	float PhaseRadius{};
	
	// �ڱ��� ���� ��ǥ �������� �پ��� �����ϱ� �������� Holding time
	float ShrinkDelayTime{};

	// �پ��� �� �ð�
	float ShrinkTotalTime{};

public:

	// ���� Phase�� ������ ���̴� �ӵ���
	float RadiusShrinkSpeed{};

	// ���� Phase�� �߾��� �ű�� �ӵ���
	float MidPointMoveSpeed{};

	// ���� Phase�� �߾��� �ű�� ����
	FVector MidPointMoveDirection{};

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

private:

	/// <summary>
	/// �ڱ��� FSM �ڵ鸵
	/// </summary>
	void HandleUpdateState(const float& DeltaTime);

protected:

	/// <summary>
	/// ù �ڱ��� Init, BluePrint Beginplay���� ȣ��� ����
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void InitManager();

	/// <summary>
	/// �������Ʈ ������ ȣ���� �Լ�
	/// </summary>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable)
	int32 GetSliceCount() const { return SLICE_COUNT; }

private:

	/// <summary>
	/// ���� ������Ʈ�� ���� Update
	/// </summary>
	/// <param name="MidLocation"> : Update��ų ���� �߾� ��ġ </param>
	/// <param name="Radius"> : Update��ų ���� ������ </param>
	void UpdateWalls(const FVector& MidLocation, const float& Radius);

	void TestUpdateWalls(const FVector& MidLocation, const float& Radius);
	
	/// <summary>
	/// Random�� ���� Next Circle �̱� & �پ��� �ӷ� ���س��� & �߾��� �ű�� ���� ���س���
	/// </summary>
	void SetRandomNextCircleAndSpeedDirection();

protected:

	// MainCircle�� In game ������ ������ ������Ʈ��
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<class AC_MagneticWall*> MagneticWalls{};

	// TODO : �� ���� ���� �� (�׽��� ��)
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<class AC_MagneticWall*> ManeticWallsTemp{};

private:

	EMagneticFieldState MagneticFieldState{};

private:
	
	// �������� �ڱ��� ���� circle
	FMagneticCircle MainCircle{};

	// ���� Phase�� �پ�� �ڱ��� ���� circle 
	FMagneticCircle NextCircle{};

private:

	// ���� �ڱ��� phase
	int CurrentPhase = 1;

	// TODO : ��Ȯ�� ������ ���߿� ����, ���� Test ��
	TMap<int, FPhaseInfo> PhaseInfos = 
	{
		{1, FPhaseInfo(15000.f, 10.f, 10.f)},	// 150m
		{2, FPhaseInfo(7000.f, 10.f, 10.f)},	// 70m
		{3, FPhaseInfo(3000.f, 10.f, 10.f)},	// 30m
		{4, FPhaseInfo(1.f, 0.f, 0.f)}			// ���� ������ ���� ���� (��ü Phase���� �ϳ� �� ���Բ� �������� ���� �۵���)
	};

	// �ð� ����
	float Timer{};

private:

	const int SLICE_COUNT	= 100;
	const int LAST_PHASE	= 3; // TODO : Last Phase ���� ����
	
};
