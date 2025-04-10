// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "C_BTTaskAfterTakeCoverMoveTo.generated.h"

enum class EPoseState : uint8;
/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BTTaskAfterTakeCoverMoveTo : public UBTTaskNode
{
	GENERATED_BODY()
public:

	UC_BTTaskAfterTakeCoverMoveTo();

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:

	// static const TMap<EPoseState, float> FloorToHeadOffsets;
	
};
