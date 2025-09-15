// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_TrainingGroundManager.generated.h"

/// <summary>
/// TrainingGround 내의 Field 위치 이름들
/// </summary>
UENUM(BlueprintType)
enum class ETrainingGroundPlace : uint8
{
	ShootingRange,
	SkyDivingField,
	GrenadePit,
	TutorialField,
	SupplyDepot,
	Garage,
	AIMovementTestField,
	AIStatCareTestField,
	EVETestField,
	PVETestField,
	Max
};

UCLASS()
class UE_PUBG_API AC_TrainingGroundManager : public AActor
{
	GENERATED_BODY()

public:
	AC_TrainingGroundManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:

	void SetPlayerToDefaultTrainingGroundTransform();

	class AC_TutorialManager* GetTutorialManager() const { return TutorialManager; }

	class AC_AISkyDiveTesterManager* GetAISkyDiveTesterManager() const { return AISkyDiveTesterManager; }

	class AC_CombatFieldManager* GetCombatFieldManager() const { return CombatFieldManager; }

	class AC_Enemy* GetMovementTester() const { return MovementTester; }

	bool AddTransformToTrainingGroundPlaceSpawnTransforms(ETrainingGroundPlace TargetPlace, const FTransform& Transform);

	bool MovePlayerToPlaceSpawnTransform(ETrainingGroundPlace TargetPlace);

protected:

	// Training ground의 가장 기본 위치 저장 (Tutorial 끝난 이 후, 해당 위치로 이동하도록 처리)
	UPROPERTY(EditInstanceOnly)
	FTransform TrainingGroundDefaultPlayerTransform{};

	UPROPERTY(EditInstanceOnly)
	AC_TutorialManager* TutorialManager{};
	
	UPROPERTY(EditInstanceOnly)
	AC_AISkyDiveTesterManager* AISkyDiveTesterManager{};

	UPROPERTY(EditInstanceOnly)
	AC_CombatFieldManager* CombatFieldManager{};

private:
	
	//  MainMap 내의 버튼으로 순간이동할 수 있는 위치 저장
	TMap<ETrainingGroundPlace, FTransform> TrainingGroundPlaceSpawnTransforms{};
	
protected:

	UPROPERTY(EditInstanceOnly)
	AC_Enemy* MovementTester{};
	
};
