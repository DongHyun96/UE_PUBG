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

	class UC_CombatFieldWidget* GetCombatFieldWidget() const { return CombatFieldWidget; }

	class UC_EnemyCombatFieldManager* GetEnemyCombatFieldManager() const { return EnemyCombatFieldManager; }
	class UC_PlayerCombatFieldManager* GetPlayerCombatFieldManager() const { return PlayerCombatFieldManager; }

	/// <returns> : Index에 맞는 VersusAIEnemy 반환, Index가 Valid하지 않다면 return nullptr </returns>
	class AC_Enemy* GetVersusAIEnemy(uint8 Index) const;

private:
	
	/// <summary>
	/// E vs E 시작 사전 준비
	/// </summary>
	void InitEnemyVsEnemyRound();

public:

	/// <summary>
	/// Enemy vs Enemy 시작하기
	/// </summary>
	void StartEnemyVsEnemyRound();

	/// <summary>
	/// Enemy Vs Enemy Round 멈추기 (둘 다 Wait 상태로 두고, TargetCharacter 해제)
	/// </summary>
	void StopEnemyVsEnemyRound();
	
protected:
	
	/// <summary>
	/// 죽은 Enemy 소생시키기 
	/// </summary>
	/// <param name="Enemy"> : 소생시키려 하는 Enemy </param>
	/// <returns> : 아직 살아있는 상태라면(MainState != Dead) return false </returns>
	bool TryReviveEnemy(class AC_Enemy* Enemy);


protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<AC_Enemy> EnemyClass{};

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

protected:

	// Enemy vs Enemy 관전 처리를 돕는 ActorComponent
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UC_EnemyCombatFieldManager* EnemyCombatFieldManager{};

	// Player vs Enemy 라운드 처리를 돕는 ActorComponent
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UC_PlayerCombatFieldManager* PlayerCombatFieldManager{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UC_CombatFieldWidget* CombatFieldWidget{};

};
