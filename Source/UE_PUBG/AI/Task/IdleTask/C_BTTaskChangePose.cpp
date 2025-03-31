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

	AC_EnemyAIController* EnemyAIController = Cast<AC_EnemyAIController>(OwnerComp.GetAIOwner());
	if (!IsValid(EnemyAIController))
	{
		UC_Util::Print("From UC_BTTaskChangePose : EnemyAIController casting failed!", FColor::Red, 10.f);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AC_Enemy* Enemy = Cast<AC_Enemy>(EnemyAIController->GetPawn());
	if (Enemy->GetIsPoseTransitioning()) return;

	// End of PoseTransition
	
	UC_Util::Print("Pose Transitioning Finished!", FColor::Red, 10.f);
	EnemyAIController->GetBehaviorComponent()->SetIdleTaskTypeToPrevType();
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	Enemy->UpdateMaxWalkSpeed({1.f, 0.f}); // Forward 방면 기준으로 MaxWalkSpeed 새로 지정
}

EBTNodeResult::Type UC_BTTaskChangePose::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetOwner());
	if (!IsValid(Controller))
	{
		UC_Util::Print("From UC_BTTaskChangePose ExecuteTask : Controller Casting failed!", FColor::Red, 10.f);
		return EBTNodeResult::Failed;
	}
	
	AC_Enemy* OwnerEnemy = Cast<AC_Enemy>(Controller->GetPawn());

	if (!IsValid(OwnerEnemy))
	{
		UC_Util::Print("From UC_BTTaskChangePose ExecuteTask : OwnerEnemy Casting failed!", FColor::Red, 10.f);
		return EBTNodeResult::Failed;
	}

	UC_BehaviorComponent* OwnerBehaviorComponent = Controller->GetBehaviorComponent();

	bool Succeeded = OwnerEnemy->SetPoseState(OwnerEnemy->GetPoseState(), OwnerBehaviorComponent->GetNextPoseState());
	if (!Succeeded)
	{
		OwnerBehaviorComponent->SetIdleTaskTypeToPrevType();
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress; // TickTask에서 PoseTransition 액션 중인지 확인
}


