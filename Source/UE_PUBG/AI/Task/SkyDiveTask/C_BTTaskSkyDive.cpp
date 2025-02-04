// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/SkyDiveTask/C_BTTaskSkyDive.h"

UC_BTTaskSkyDive::UC_BTTaskSkyDive()
{
}

void UC_BTTaskSkyDive::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

EBTNodeResult::Type UC_BTTaskSkyDive::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// TODO : 목적지 지정하기
	Super::ExecuteTask(OwnerComp, NodeMemory);

	return EBTNodeResult::Type();
}
