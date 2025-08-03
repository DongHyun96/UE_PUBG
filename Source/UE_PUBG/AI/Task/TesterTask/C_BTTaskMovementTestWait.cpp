// Fill out your copyright notice in the Description page of Project Settings.


#include "C_BTTaskMovementTestWait.h"

#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"

void UC_BTTaskMovementTestWait::OnWaitTimeRemain(UBehaviorTreeComponent& OwnerComp,
                                                 AC_EnemyAIController* EnemyAIController, AC_Enemy* Enemy, UC_BehaviorComponent* EnemyBehaviorComponent)
{
	// Blank 처리 (부모클래스의 함수에서는 In game play FSM 처리 일부를 맡았음)
}

void UC_BTTaskMovementTestWait::OnWaitTimeFinished(UBehaviorTreeComponent& OwnerComp,
	AC_EnemyAIController* EnemyAIController, AC_Enemy* Enemy, UC_BehaviorComponent* EnemyBehaviorComponent)
{
	EnemyTimers[EnemyBehaviorComponent] -= EnemyAIController->GetBehaviorComponent()->GetWaitTime();
	
	// 다음 Random Movement 처리
	ExecuteMoveToRandomLocation(Enemy, 1000.f, false);
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
