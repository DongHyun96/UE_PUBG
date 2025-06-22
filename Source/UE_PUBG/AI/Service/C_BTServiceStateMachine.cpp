// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/C_BTServiceStateMachine.h"

#include "C_BTServiceCombat.h"
#include "AI/C_EnemyAIController.h"
#include "AI/C_BehaviorComponent.h"
#include "AI/Service/C_BTServiceIdle.h"

#include "Character/C_Enemy.h"
#include "Character/Component/C_SmokeEnteredChecker.h"
#include "Character/Component/EnemyComponent/C_TargetLocationSettingHelper.h"

#include "Character/Component/SkyDivingComponent/C_SkyDivingComponent.h"
#include "MagneticField/C_MagneticFieldManager.h"
#include "Singleton/C_GameSceneManager.h"

UC_BTServiceStateMachine::UC_BTServiceStateMachine()
{
	bNotifyTick = true;
}

void UC_BTServiceStateMachine::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// UC_Util::Print("State Machine");

	AC_EnemyAIController*	EnemyAIController		= Cast<AC_EnemyAIController>(OwnerComp.GetAIOwner());
	AC_Enemy*				Enemy					= Cast<AC_Enemy>(EnemyAIController->GetPawn());
	UC_BehaviorComponent*	EnemyBehaviorComponent	= EnemyAIController->GetBehaviorComponent();
	
	if (Enemy->GetMainState() == EMainState::SKYDIVING || Enemy->GetMainState() == EMainState::DEAD) return;
	if (EnemyAIController->IsFlashBangEffectTimeLeft())	return; // 현재 FlashBang 피격 중인 상황(현상 유지)
	
	if (!EnemyTimers.Contains(EnemyBehaviorComponent)) EnemyTimers.Add(EnemyBehaviorComponent, 0.f);

	// Update Detected Characters' Sight Range Level
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
			// InCircle targetLocation은 이미 잡혀 있음
			EnemyBehaviorComponent->SetServiceType(EServiceType::IDLE);
			EnemyBehaviorComponent->SetIdleTaskType(EIdleTaskType::INCIRCLE_MOVETO);
			return;
		}
	}

	bool bIsInSmokeArea = Enemy->GetSmokeEnteredChecker()->IsCurrentlyInSmokeArea();
	
	// Lv1 영역으로 다른 캐릭터가 들어왔다면 TargetCharacter로 set해서 공격 시도
	if (EnemyAIController->TrySetTargetCharacterToLevel1EnteredCharacter() && !bIsInSmokeArea)
	{
		EnemyBehaviorComponent->SetServiceType(EServiceType::COMBAT);
		return;
	}

	// 피 또는 부스트 량이 너무 없을 때 처리(STAT_CARE)
	if (Enemy->GetStatComponent()->GetCurHP() < 50.f || Enemy->GetStatComponent()->GetCurBoosting() < 50.f)
	{
		EnemyBehaviorComponent->SetServiceType(EServiceType::IDLE);
		EnemyBehaviorComponent->SetIdleTaskType(EIdleTaskType::STAT_CARE);
		return;
	}

	EnemyTimers[EnemyBehaviorComponent] += DeltaSeconds;
	if (EnemyTimers[EnemyBehaviorComponent] < EnemyBehaviorComponent->GetWaitTime()) return;
	
	// Wait Time이 모두 끝났을 때
	
	EnemyTimers[EnemyBehaviorComponent] = 0.f;

	// MoveToRandomPos or Attack trial or 다시 Wait 중 택 1
	// Attack Trial 40% | MoveToRandomPos 30% | Wait 30%

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// SetTargetCharacter & AttackTrial -> 무조건 공격 testing 했을 때 사용했었던 코드
	/*EnemyAIController->TrySetTargetCharacterBasedOnPriority();
	if (IsValid(EnemyBehaviorComponent->GetTargetCharacter()) && !bIsInSmokeArea)
	{
		UC_Util::Print("WaitTask Time's up : Try Attack TargetCharacter!", FColor::Red, 10.f);
		EnemyBehaviorComponent->SetServiceType(EServiceType::COMBAT);
		// return;
	}*/
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (FMath::RandRange(0.f, 1.f) < 0.4f && !bIsInSmokeArea) // SetTargetCharacter & AttackTrial
	{
		EnemyAIController->TrySetTargetCharacterBasedOnPriority();
		if (IsValid(EnemyBehaviorComponent->GetTargetCharacter()))
		{
			// UC_Util::Print("WaitTask Time's up : Try Attack TargetCharacter!", FColor::Red, 10.f);
			EnemyBehaviorComponent->SetServiceType(EServiceType::COMBAT);
			return;
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (FMath::RandRange(0, 1)) // MoveToRandomPos
	{
		ExecuteMoveToRandomLocation(Enemy, EnemyBehaviorComponent, true);
		return;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// UC_Util::Print("WaitTask Time's up : Keep waiting...", FColor::Red, 10.f);
	EnemyBehaviorComponent->SetIdleTaskType(EIdleTaskType::WAIT); // 다시금 기다리기 처리(이 호출로 기다리는 총 시간 랜덤하게 다시 setting 됨)
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

void UC_BTServiceStateMachine::ExecuteMoveToRandomLocation(AC_Enemy* Enemy, UC_BehaviorComponent* EnemyBehaviorComponent, bool bMoveRandomlyInsideMainCircle)
{
	if (bMoveRandomlyInsideMainCircle)	Enemy->GetTargetLocationSettingHelper()->SetRandomBasicTargetLocationInsideMainCircle(1000.f);
	else								Enemy->GetTargetLocationSettingHelper()->SetRandomTargetLocation(1000.f);
	
	EnemyBehaviorComponent->SetIdleTaskType(EIdleTaskType::BASIC_MOVETO);
}
