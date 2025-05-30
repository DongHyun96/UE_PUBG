﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "C_BTTaskWaitAfterAttack.h"

#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"
#include "Character/C_Enemy.h"
#include "Utility/C_Util.h"

UC_BTTaskWaitAfterAttack::UC_BTTaskWaitAfterAttack()
{
	bNotifyTick = true;
}

void UC_BTTaskWaitAfterAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AC_EnemyAIController*	EnemyAIController		= Cast<AC_EnemyAIController>(OwnerComp.GetAIOwner());
	AC_Enemy*				Enemy					= Cast<AC_Enemy>(EnemyAIController->GetPawn());
	UC_BehaviorComponent*	EnemyBehaviorComponent	= EnemyAIController->GetBehaviorComponent();

	EnemyTimers[Enemy] += DeltaSeconds;

	if (EnemyTimers[Enemy] < EnemyBehaviorComponent->GetAfterAttackTaskWaitTime()) return;
	
	// 기다린 시간 끝
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	EnemyBehaviorComponent->SetIdleTaskType(EIdleTaskType::WAIT);
	EnemyBehaviorComponent->SetServiceType(EServiceType::IDLE);
}

EBTNodeResult::Type UC_BTTaskWaitAfterAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetOwner());
	if (!IsValid(Controller))
	{
		UC_Util::Print("From UC_BTTaskWaitAfterAttack::ExecuteTask : Controller Casting failed!", FColor::Red, 10.f);
		return EBTNodeResult::Failed;
	}
	
	AC_Enemy* Enemy = Cast<AC_Enemy>(Controller->GetPawn());
	if (!Enemy)
	{
		UC_Util::Print("From UC_BTTaskWaitAfterAttack::ExecuteTask : Enemy Actor Casting failed!", FColor::Red, 10.f);
		return EBTNodeResult::Failed;
	}
	
	UC_BehaviorComponent* BehaviorComponent = Controller->GetBehaviorComponent();

	if (BehaviorComponent->GetAfterAttackTaskWaitTime() <= 0.f)
	{
		BehaviorComponent->SetIdleTaskType(EIdleTaskType::WAIT);
		BehaviorComponent->SetServiceType(EServiceType::IDLE);
		return EBTNodeResult::Succeeded;
	}

	EnemyTimers.Add(Enemy, 0.f);
	
	return EBTNodeResult::InProgress;
}
