// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "C_BTTaskWaitAfterAttack.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BTTaskWaitAfterAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UC_BTTaskWaitAfterAttack();

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:

	TMap<class AC_Enemy*, float> EnemyTimers{};

};
