// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/C_BTServiceStateMachine.h"

#include "AI/C_EnemyAIController.h"
#include "AI/C_BehaviorComponent.h"
#include "AI/Service/C_BTServiceIdle.h"

#include "Character/C_Enemy.h"

#include "Character/Component/C_SkyDivingComponent.h"

#include "Utility/C_Util.h"


UC_BTServiceStateMachine::UC_BTServiceStateMachine()
{
}

void UC_BTServiceStateMachine::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UC_Util::Print("UC_BTServiceStateMachine::TickNode");

	//AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetOwner());

	if (!IsValid(OwnerController))
	{
		OwnerController			= Cast<AC_EnemyAIController>(OwnerComp.GetOwner());
		OwnerEnemy				= Cast<AC_Enemy>(OwnerController->GetPawn());
		OwnerBehaviorComponent	= OwnerController->GetBehaviorComponent();
	}

	// 총괄 FSM 및 기타 Main 처리 담당하기
	if (OwnerEnemy->GetMainState() == EMainState::SKYDIVING)
		OwnerBehaviorComponent->SetServiceType(EServiceType::SKYDIVE);

	/*// Testing Pose Transition
	Timer += DeltaSeconds;

	if (Timer > 3.f) // Set IdleTaskType to PoseChange
	{
		Timer = 0.f;
		OwnerBehaviorComponent->SetIdleTaskType(EIdleTaskType::CHANGE_POSE);
	}
	else OwnerBehaviorComponent->SetIdleTaskType(EIdleTaskType::WAIT);
	*/
		
}


