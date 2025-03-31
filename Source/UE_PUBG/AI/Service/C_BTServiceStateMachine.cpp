// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/C_BTServiceStateMachine.h"

#include "AI/C_EnemyAIController.h"
#include "AI/C_BehaviorComponent.h"
#include "AI/Service/C_BTServiceIdle.h"

#include "Character/C_Enemy.h"

#include "Character/Component/SkyDivingComponent/C_SkyDivingComponent.h"
#include "Singleton/C_GameSceneManager.h"

#include "Utility/C_Util.h"


UC_BTServiceStateMachine::UC_BTServiceStateMachine()
{
	bNotifyTick = true;
}

void UC_BTServiceStateMachine::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AC_EnemyAIController* OwnerController = Cast<AC_EnemyAIController>(OwnerComp.GetAIOwner());
	AC_Enemy* OwnerEnemy = Cast<AC_Enemy>(OwnerController->GetPawn());
	UC_BehaviorComponent* OwnerBehaviorComponent = OwnerController->GetBehaviorComponent(); 
	
	// 총괄 FSM 및 기타 Main 처리 담당하기
	if (OwnerEnemy->GetMainState() == EMainState::SKYDIVING)
		OwnerBehaviorComponent->SetServiceType(EServiceType::SKYDIVE);

	// TODO : RangeLevel 업데이트 -> 행동 취하기 전에만 Update해서 계산해주면 됨 (이 라인은 테스트용)
	OwnerController->UpdateDetectedCharactersRangeLevel();
	
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
