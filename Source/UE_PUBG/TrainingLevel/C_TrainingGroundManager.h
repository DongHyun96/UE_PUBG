// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_TrainingGroundManager.generated.h"

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

	class AC_AISkyDiveTesterManager* GetAISkyDiveTesterManager() const { return AISkyDiveTesterManager; }

	class AC_CombatFieldManager* GetCombatFieldManager() const { return CombatFieldManager; }

protected:

	// Training ground의 가장 기본 위치 저장 (Tutorial 끝난 이 후, 해당 위치로 이동하도록 처리)
	UPROPERTY(EditInstanceOnly)
	FTransform TrainingGroundDefaultPlayerTransform{};

	UPROPERTY(EditInstanceOnly)
	AC_AISkyDiveTesterManager* AISkyDiveTesterManager{};

	UPROPERTY(EditInstanceOnly)
	AC_CombatFieldManager* CombatFieldManager{};	
};
