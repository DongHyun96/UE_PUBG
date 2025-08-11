// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_CombatFieldManager.generated.h"

/// <summary>
/// <para> TrainingGround내의 Combat Field 관장 </para>
/// <para> Enemy vs Enemy Field | Player vs Enemy Field 통합 관장 </para>
/// </summary>
UCLASS()
class UE_PUBG_API AC_CombatFieldManager : public AActor
{
	GENERATED_BODY()

public:
	
	AC_CombatFieldManager();

protected:
	
	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<class AC_Enemy> EnemyClass{};

protected:

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly)
	TArray<AC_Enemy*> VersusAIEnemies{};   // Enemy vs Enemy Field Enemies

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly)
	AC_Enemy*         VersusPlayerEnemy{}; // Player vs Enemy 용 Enemy

private: // Spawn(Respawn) Transform 관련

	// Spawn Transform 저장용
	TArray<FTransform> EnemyVsEnemySpawnTransform{};
	TArray<FTransform> PlayerVsEnemySpawnTransform{};
};
