// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "C_BTTaskMoveTo.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BTTaskMoveTo : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:

	UC_BTTaskMoveTo();

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	
};
