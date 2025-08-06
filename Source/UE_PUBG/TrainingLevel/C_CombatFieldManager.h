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

private:

	FActorSpawnParameters SpawnParameters{};
	
private:

	// Enemy vs Enemy Field Enemies
	TArray<AC_Enemy*> VersusAIEnemies{};

	// Player vs Enemy 용 Enemy
	AC_Enemy* VersusPlayerEnemy{};

private: // Spawn(Respawn) Transform

	TArray<class UCapsuleComponent*> EnemyVsEnemySpawnTransform{};
	
	TArray<UCapsuleComponent*> PlayerVsEnemySpawnTransform{};
};
