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


public:
	
	/// <summary>
	/// Enemy vs Enemy Round 다시 초기화 처리 및 재시작 (E Vs E Enemy 한 명이라도 사망 처리된 이후 Destroy될 때 호출) 
	/// </summary>
	/// <returns> : Init 할 수 없는 상황이라면(둘 다 아직 살아있는 경우), return false </returns>
	bool RestartEnemyVsEnemyRound();

	/// <summary>
	/// E vs E 첫 시작 처리
	/// </summary>
	void StartEnemyVsEnemyRound();

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

	FTransform EnemyMeshInitialRelativeTransform{};

};
