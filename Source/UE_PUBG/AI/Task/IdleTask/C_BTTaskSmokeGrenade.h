// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "C_BTTaskSmokeGrenade.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BTTaskSmokeGrenade : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UC_BTTaskSmokeGrenade();

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:

private:

	// 특정 Enemy의 Smoke Grenade Execute AIAttackStrategy 시작되었는지 체크하는 용도
	TSet<class AC_Enemy*> SmokeGrenadeAIAttackExecutedEnemies{};

	// ChangingWeapon false 이후, 시간 재기 timer
	TMap<AC_Enemy*, float> EnemyTickTaskTimer{};
};
