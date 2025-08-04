// Fill out your copyright notice in the Description page of Project Settings.


#include "C_BTTaskStatCareTestWait.h"

#include "AI/C_BehaviorComponent.h"
#include "Character/C_Enemy.h"
#include "Utility/C_Util.h"

void UC_BTTaskStatCareTestWait::OnWaitTimeRemain(UBehaviorTreeComponent& OwnerComp,
                                                 AC_EnemyAIController* EnemyAIController, AC_Enemy* Enemy, UC_BehaviorComponent* EnemyBehaviorComponent)
{
	// 피 또는 부스트 량이 너무 없을 때 처리(STAT_CARE)
	if (Enemy->GetStatComponent()->GetCurHP() < 50.f || Enemy->GetStatComponent()->GetCurBoosting() < 50.f)
	{
		EnemyBehaviorComponent->SetIdleTaskType(EIdleTaskType::STAT_CARE);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UC_BTTaskStatCareTestWait::OnWaitTimeFinished(UBehaviorTreeComponent& OwnerComp,
	AC_EnemyAIController* EnemyAIController, AC_Enemy* Enemy, UC_BehaviorComponent* EnemyBehaviorComponent)
{
	if (!EnemyTimers.Contains(EnemyBehaviorComponent))
	{
		UC_Util::Print("From UC_BTTaskStatCareTestWait::OnWaitTimeFinished : EnemyTimers does not contains received EnemyBehaviorComponent!", FColor::Red, 10.f);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	EnemyTimers[EnemyBehaviorComponent] -= EnemyBehaviorComponent->GetWaitTime();
	
	// 계속 기다리기 처리 (직접적인 공격을 당했을 때, TakeCover - StatCare 처리가 외부에서 setting됨(Enemy StatComponent TakeDamage))
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
