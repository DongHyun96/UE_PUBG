// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/C_BTServiceIdle.h"

UC_BTServiceIdle::UC_BTServiceIdle()
{
	NodeName = "Idle";
}

void UC_BTServiceIdle::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	/*AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetOwner());

	if (!IsValid(Controller))
	{
		UC_Util::Print("From BTService Idle TickNode : Controller casting failed!", FColor::Red, 10.f);
		return;
	}

	UC_BehaviorComponent* BehaviorComponent = Controller->GetBehaviorComponent();

	if (!IsValid(BehaviorComponent))
	{
		UC_Util::Print("From BTService Idle TickNode : BehaviorComponent casting failed!", FColor::Red, 10.f);
		return;
	}
	*/

	// TODO : Idle Service 관련 처리
}


