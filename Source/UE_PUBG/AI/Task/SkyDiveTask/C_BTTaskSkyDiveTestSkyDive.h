// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "C_BTTaskSkyDiveTestSkyDive.generated.h"

/**
 * SkyDive Tester AI 용 SkyDive Task node
 */
UCLASS()
class UE_PUBG_API UC_BTTaskSkyDiveTestSkyDive : public UBTTaskNode
{
	GENERATED_BODY()

	UC_BTTaskSkyDiveTestSkyDive();

public:

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
