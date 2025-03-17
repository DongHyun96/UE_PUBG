// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/IdleTask/C_BTTaskMoveTo.h"

#include "Utility/C_Util.h"

UC_BTTaskMoveTo::UC_BTTaskMoveTo()
{
	bNotifyTick = true;
}

void UC_BTTaskMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// TODO : 너무 한 자리에 머무는 시간이 길어지면 MoveTo하는 중 막혔다고 판단, 다른 루트로 업데이트 시키기
}

EBTNodeResult::Type UC_BTTaskMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
