// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "C_BTTaskWait.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BTTaskWait : public UBTTask_Wait
{
	GENERATED_BODY()

public:

	UC_BTTaskWait();

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:

	class AC_Enemy*				OwnerEnemy{};
	class UC_BehaviorComponent* OwnerBehaviorComponent{};
	
};
