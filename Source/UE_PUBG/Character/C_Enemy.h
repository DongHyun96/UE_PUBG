// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/C_BasicCharacter.h"
#include "C_Enemy.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_Enemy : public AC_BasicCharacter
{
	GENERATED_BODY()

public:
	AC_Enemy();

	void BeginPlay() override;

	void Tick(float DeltaSeoncds) override;


	class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

public:

	bool SetPoseState(EPoseState InChangeFrom, EPoseState InChangeTo) override;

private:

	/// <summary>
	/// 기본 무기 및 기본 아이템 비행기 탑승 시 가지고 시작
	/// </summary>
	void SpawnDefaultWeaponsAndItemsForSelf();

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UBehaviorTree* BehaviorTree{};

};


