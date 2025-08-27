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

	static void SetCharacterMeshInitialRelativeTransform(const FTransform& Transform) { CharacterMeshInitialRelativeTransform = Transform; }

	class UC_CombatFieldWidget* GetCombatFieldWidget() const { return CombatFieldWidget; }

	class UC_EnemyCombatFieldManager* GetEnemyCombatFieldManager() const { return EnemyCombatFieldManager; }
	class UC_PlayerCombatFieldManager* GetPlayerCombatFieldManager() const { return PlayerCombatFieldManager; }

public:
	
	/// <summary>
	/// Combat Character Round 시작 전, 사전 초기화 일괄 처리 
	/// </summary>
	/// <param name="CombatCharacter"> : Init시킬 Character </param>
	/// <param name="SpawnTransform"> : Spawn 처리될 Transform </param>
	void InitRoundForCombatCharacter(class AC_BasicCharacter* CombatCharacter, const FTransform& SpawnTransform);

private:

	/// <summary>
	/// <para> Round 시작 전, Equipment와 Inven 초기화 처리 </para>
	/// <para> 주의 : Matching에 맞는 기본 아이템 종류로 세팅되어 있는 Character를 Round 시작 전 초기화 시킴 </para>
	/// </summary>
	/// <param name="Character"></param>
	void InitRoundStartEquipmentAndInven(AC_BasicCharacter* Character);

	/// <summary>
	/// 죽은 Character 소생시도
	/// </summary>
	/// <param name="Character"> : 소생시킬 Character </param>
	/// <returns> : 살아있는 상태라면 return false </returns>
	bool TryReviveCharacter(AC_BasicCharacter* Character);
	
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

private:

	class UC_DefaultItemSpawnerComponent* ItemSpawnHelper{};

private:

	static FTransform CharacterMeshInitialRelativeTransform;

};
