// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/C_BTServiceStateMachine.h"

#include "AI/C_EnemyAIController.h"
#include "AI/C_BehaviorComponent.h"
#include "AI/Service/C_BTServiceIdle.h"

#include "Character/C_Enemy.h"

#include "Character/Component/SkyDivingComponent/C_SkyDivingComponent.h"
#include "MagneticField/C_MagneticFieldManager.h"
#include "Singleton/C_GameSceneManager.h"

#include "Utility/C_Util.h"


UC_BTServiceStateMachine::UC_BTServiceStateMachine()
{
	bNotifyTick = true;
}

void UC_BTServiceStateMachine::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AC_EnemyAIController*	EnemyAIController		= Cast<AC_EnemyAIController>(OwnerComp.GetAIOwner());
	AC_Enemy*				Enemy					= Cast<AC_Enemy>(EnemyAIController->GetPawn());
	UC_BehaviorComponent*	EnemyBehaviorComponent	= EnemyAIController->GetBehaviorComponent();

	if (!EnemyTimers.Contains(EnemyBehaviorComponent)) EnemyTimers.Add(EnemyBehaviorComponent, 0.f);

	// TODO : RangeLevel 업데이트 -> 행동 취하기 전에만 Update해서 계산해주면 됨 (이 라인은 테스트용)
	EnemyAIController->UpdateDetectedCharactersRangeLevel();
	
	// Wait 상황의 FSM transition 처리하기
	
	EServiceType CurrentServiceType = EnemyBehaviorComponent->GetServiceType();
	EIdleTaskType CurrentIdleTaskType = EnemyBehaviorComponent->GetIdleTaskType();

	if (CurrentServiceType != EServiceType::IDLE || CurrentIdleTaskType != EIdleTaskType::WAIT)
	{
		EnemyTimers[EnemyBehaviorComponent] = 0.f;
		return;
	}
	
	// WAIT 중인 상황

	// Wait 상황에 자기장 밖이라면 안전지대로 이동
	AC_MagneticFieldManager* MagneticFieldManager = GAMESCENE_MANAGER->GetMagneticFieldManager();
	if (IsValid(MagneticFieldManager))
	{
		if (!MagneticFieldManager->IsInMainCircle(Enemy))
		{
			EnemyBehaviorComponent->SetIdleTaskType(EIdleTaskType::INCIRCLE_MOVETO);
			return;
		}
	}

	// TODO : 피 또는 부스트 량이 너무 없을 때 처리(STAT_CARE) and return

	EnemyTimers[EnemyBehaviorComponent] += DeltaSeconds;

	if (EnemyTimers[EnemyBehaviorComponent] > EnemyBehaviorComponent->GetWaitTime())
	{
		// Wait Time이 모두 끝났을 때
		EnemyTimers[EnemyBehaviorComponent] = 0.f;

		// TODO : MoveToRandomPos or Attack trial or 다시 Wait 중 택 1
	}
		
}
