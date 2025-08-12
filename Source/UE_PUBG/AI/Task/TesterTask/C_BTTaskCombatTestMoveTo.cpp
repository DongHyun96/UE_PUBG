// Fill out your copyright notice in the Description page of Project Settings.


#include "C_BTTaskCombatTestMoveTo.h"

#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"
#include "Character/C_BasicCharacter.h"

UC_BTTaskCombatTestMoveTo::UC_BTTaskCombatTestMoveTo()
{
}

void UC_BTTaskCombatTestMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetAIOwner());

	AC_BasicCharacter* TargetCharacter = Controller->GetBehaviorComponent()->GetTargetCharacter();

	// 공격 지점 이동 중, Combat Test TargetCharacter가 사망 시, Combat sequence 종료
	if (!TargetCharacter || TargetCharacter->GetMainState() == EMainState::DEAD)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

	// 공격 지점 이동 중, TargetCharacter가 시야에 잡힌 상황
	if (Controller->IsCurrentlyOnSight(TargetCharacter))
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

EBTNodeResult::Type UC_BTTaskCombatTestMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
