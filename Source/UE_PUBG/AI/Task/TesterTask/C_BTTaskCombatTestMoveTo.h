// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Task/IdleTask/C_BTTaskMoveTo.h"
#include "C_BTTaskCombatTestMoveTo.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BTTaskCombatTestMoveTo : public UC_BTTaskMoveTo
{
	GENERATED_BODY()

public:

	UC_BTTaskCombatTestMoveTo();

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
