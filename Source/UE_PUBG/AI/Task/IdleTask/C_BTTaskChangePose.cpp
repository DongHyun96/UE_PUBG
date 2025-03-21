// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/IdleTask/C_BTTaskChangePose.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Enemy.h"

#include "AI/C_EnemyAIController.h"
#include "AI/C_BehaviorComponent.h"

#include "Utility/C_Util.h"

UC_BTTaskChangePose::UC_BTTaskChangePose()
{
	bNotifyTick = true;
}

void UC_BTTaskChangePose::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!OwnerEnemy->GetIsPoseTransitioning())
	{
		UC_Util::Print("Pose Transitioning Finished!", FColor::Red, 10.f);
		OwnerBehaviorComponent->SetIdleTaskTypeToPrevType();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	
}

EBTNodeResult::Type UC_BTTaskChangePose::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// Initialization
	if (!IsValid(OwnerEnemy))
	{
		AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetOwner());
		if (!IsValid(Controller))
		{
			UC_Util::Print("From UC_BTTaskChangePose ExecuteTask : Controller Casting failed!", FColor::Red, 10.f);
			return EBTNodeResult::Failed;
		}

		OwnerEnemy = Cast<AC_Enemy>(Controller->GetPawn());

		if (!IsValid(OwnerEnemy))
		{
			UC_Util::Print("From UC_BTTaskChangePose ExecuteTask : OwnerEnemy Casting failed!", FColor::Red, 10.f);
			return EBTNodeResult::Failed;
		}

		OwnerBehaviorComponent = Controller->GetBehaviorComponent();

		if (!IsValid(OwnerBehaviorComponent))
		{
			UC_Util::Print("From UC_BTTaskChangePose ExecuteTask : OwnerBehaviorComponent Casting failed!", FColor::Red, 10.f);
			return EBTNodeResult::Failed;
		}
	}

	bool Succeeded = OwnerEnemy->SetPoseState(OwnerEnemy->GetPoseState(), OwnerBehaviorComponent->GetNextPoseState());
	if (!Succeeded)
	{
		OwnerBehaviorComponent->SetIdleTaskTypeToPrevType();
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress; // TickTask에서 PoseTransition 액션 중인지 확인
}


