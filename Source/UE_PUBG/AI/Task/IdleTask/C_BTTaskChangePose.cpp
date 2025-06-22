// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/IdleTask/C_BTTaskChangePose.h"
#include "AI/C_BehaviorComponent.h"

UC_BTTaskChangePose::UC_BTTaskChangePose()
{
	bNotifyTick = true;
}

void UC_BTTaskChangePose::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	/*AC_EnemyAIController* EnemyAIController = Cast<AC_EnemyAIController>(OwnerComp.GetAIOwner());
	if (!IsValid(EnemyAIController))
	{
		UC_Util::Print("From UC_BTTaskChangePose : EnemyAIController casting failed!", FColor::Red, 10.f);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AC_Enemy* Enemy = Cast<AC_Enemy>(EnemyAIController->GetPawn());
	if (Enemy->GetIsPoseTransitioning()) return;

	// End of PoseTransition, 또는 이미 해당 자세였을 때
	
	UC_Util::Print("Pose Transitioning Finished!", FColor::Red, 10.f);
	EnemyAIController->GetBehaviorComponent()->SetIdleTaskTypeToPrevType();
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	Enemy->UpdateMaxWalkSpeed({1.f, 0.f}); // Forward 방면 기준으로 MaxWalkSpeed 새로 지정*/
}

EBTNodeResult::Type UC_BTTaskChangePose::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::ExecuteTask(OwnerComp, NodeMemory);
	
	/*Super::ExecuteTask(OwnerComp, NodeMemory);

	AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetOwner());
	if (!IsValid(Controller))
	{
		UC_Util::Print("From UC_BTTaskChangePose ExecuteTask : Controller Casting failed!", FColor::Red, 10.f);
		return EBTNodeResult::Failed;
	}
	
	AC_Enemy* Enemy = Cast<AC_Enemy>(Controller->GetPawn());

	if (!IsValid(Enemy))
	{
		UC_Util::Print("From UC_BTTaskChangePose ExecuteTask : Enemy Casting failed!", FColor::Red, 10.f);
		return EBTNodeResult::Failed;
	}

	UC_BehaviorComponent* BehaviorComponent = Controller->GetBehaviorComponent();

	// 바꿔야할 자세가 현재 자세와 같다면, return succeeded 처리
	if (Enemy->GetPoseState() == BehaviorComponent->GetNextPoseState())
	{
		BehaviorComponent->SetIdleTaskTypeToPrevType();
		Enemy->UpdateMaxWalkSpeed({1.f, 0.f});
		return EBTNodeResult::Succeeded;
	}

	bool Succeeded = Enemy->SetPoseState(Enemy->GetPoseState(), BehaviorComponent->GetNextPoseState());
	if (!Succeeded)
	{
		BehaviorComponent->SetIdleTaskTypeToPrevType();
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress; // TickTask에서 PoseTransition 액션 중인지 확인*/
}


