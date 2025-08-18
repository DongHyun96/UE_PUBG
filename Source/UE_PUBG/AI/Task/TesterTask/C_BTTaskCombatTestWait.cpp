// Fill out your copyright notice in the Description page of Project Settings.


#include "C_BTTaskCombatTestWait.h"

#include "AI/C_BehaviorComponent.h"
#include "Character/C_BasicCharacter.h"
#include "Utility/C_Util.h"

void UC_BTTaskCombatTestWait::OnWaitTimeRemain(UBehaviorTreeComponent& OwnerComp,
                                               AC_EnemyAIController* EnemyAIController, AC_Enemy* Enemy, UC_BehaviorComponent* EnemyBehaviorComponent)
{
	// Blank 처리
}

void UC_BTTaskCombatTestWait::OnWaitTimeFinished(UBehaviorTreeComponent& OwnerComp,
	AC_EnemyAIController* EnemyAIController, AC_Enemy* Enemy, UC_BehaviorComponent* EnemyBehaviorComponent)
{
	if (!EnemyTimers.Contains(EnemyBehaviorComponent))
	{
		UC_Util::Print("From UC_BTTaskCombatTestWait::OnWaitTimeFinished : EnemyTimers does not contains received EnemyBehaviorComponent!", FColor::Red, 10.f);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	EnemyTimers[EnemyBehaviorComponent] -= EnemyBehaviorComponent->GetWaitTime();

	const AC_BasicCharacter* TargetCharacter = EnemyBehaviorComponent->GetTargetCharacter();
	if (IsValid(TargetCharacter) && TargetCharacter->GetMainState() != EMainState::DEAD) // TargetCharacter가 valid할 때에만 Combat Service로 넘어감
	{
		EnemyBehaviorComponent->SetServiceType(EServiceType::COMBAT);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
