// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/C_BTServiceStateMachine.h"

#include "AI/C_EnemyAIController.h"
#include "AI/C_BehaviorComponent.h"

#include "Utility/C_Util.h"

UC_BTServiceStateMachine::UC_BTServiceStateMachine()
{
}

void UC_BTServiceStateMachine::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	//AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetOwner());

	if (!IsValid(OwnerController)) OwnerController = Cast<AC_EnemyAIController>(OwnerComp.GetOwner());
	if (!IsValid(OwnerBehaviorComponent)) OwnerBehaviorComponent = OwnerController->GetBehaviorComponent();

	// TODO : �Ѱ� FSM �� ��Ÿ Main ó�� ����ϱ�
	OwnerBehaviorComponent->SetServiceType(EServiceType::COMBAT);
}
