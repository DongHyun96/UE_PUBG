// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/IdleTask/C_BTTaskWait.h"

#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"
#include "Airplane/C_AirplaneManager.h"
#include "Character/C_Enemy.h"
#include "Character/Component/C_SmokeEnteredChecker.h"
#include "Character/Component/EnemyComponent/C_TargetLocationSettingHelper.h"
#include "MagneticField/C_MagneticFieldManager.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"

UC_BTTaskWait::UC_BTTaskWait()
{
	bNotifyTick = true;
}

void UC_BTTaskWait::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AC_EnemyAIController*	EnemyAIController		= Cast<AC_EnemyAIController>(OwnerComp.GetAIOwner());
	AC_Enemy*				Enemy					= Cast<AC_Enemy>(EnemyAIController->GetPawn());
	UC_BehaviorComponent*	EnemyBehaviorComponent	= EnemyAIController->GetBehaviorComponent();
	
	if (Enemy->GetMainState() == EMainState::SKYDIVING || Enemy->GetMainState() == EMainState::DEAD) return;
	if (EnemyAIController->IsFlashBangEffectTimeLeft())	return; // 현재 FlashBang 피격 중인 상황(현상 유지)
	
	if (!EnemyTimers.Contains(EnemyBehaviorComponent)) EnemyTimers.Add(EnemyBehaviorComponent, 0.f);

	// Wait 상황의 FSM transition 처리하기
	
	EServiceType CurrentServiceType   = EnemyBehaviorComponent->GetServiceType();
	EIdleTaskType CurrentIdleTaskType = EnemyBehaviorComponent->GetIdleTaskType();

	if (CurrentServiceType != EServiceType::IDLE || CurrentIdleTaskType != EIdleTaskType::WAIT)
	{
		EnemyTimers[EnemyBehaviorComponent] = 0.f;
		return;
	}

	// WAIT 중인 상황
	OnWaitTimeRemain(OwnerComp, EnemyAIController, Enemy, EnemyBehaviorComponent);

	EnemyTimers[EnemyBehaviorComponent] += DeltaSeconds;
	if (EnemyTimers[EnemyBehaviorComponent] < EnemyBehaviorComponent->GetWaitTime()) return;
	
	// Wait Time이 모두 끝났을 때
	OnWaitTimeFinished(OwnerComp, EnemyAIController, Enemy, EnemyBehaviorComponent);
}

EBTNodeResult::Type UC_BTTaskWait::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	return EBTNodeResult::InProgress;
}

void UC_BTTaskWait::OnWaitTimeRemain(UBehaviorTreeComponent& OwnerComp, AC_EnemyAIController* EnemyAIController, AC_Enemy* Enemy, UC_BehaviorComponent* EnemyBehaviorComponent)
{
	AC_AirplaneManager* AirplaneManager = GAMESCENE_MANAGER->GetAirplaneManager();
	if (AirplaneManager) if (!AirplaneManager->GetHasAirplaneTakeOff()) return;
	
	// Wait 상황에 자기장 밖이라면 안전지대로 이동
	AC_MagneticFieldManager* MagneticFieldManager = GAMESCENE_MANAGER->GetMagneticFieldManager();
	if (IsValid(MagneticFieldManager))
	{
		if (!MagneticFieldManager->IsInMainCircle(Enemy))
		{
			// InCircle targetLocation은 이미 잡혀 있음
			EnemyBehaviorComponent->SetServiceType(EServiceType::IDLE);
			EnemyBehaviorComponent->SetIdleTaskType(EIdleTaskType::INCIRCLE_MOVETO);
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}

	bool bIsInSmokeArea = Enemy->GetSmokeEnteredChecker()->IsCurrentlyInSmokeArea();

	// Update Detected Characters' Sight Range Level
	EnemyAIController->UpdateDetectedCharactersRangeLevel();
	
	// Lv1 영역으로 다른 캐릭터가 들어왔다면 TargetCharacter로 set해서 공격 시도
	if (EnemyAIController->TrySetTargetCharacterToLevel1EnteredCharacter() && !bIsInSmokeArea)
	{
		EnemyBehaviorComponent->SetServiceType(EServiceType::COMBAT);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	// 피 또는 부스트 량이 너무 없을 때 처리(STAT_CARE)
	if (Enemy->GetStatComponent()->GetCurHP() < 50.f || Enemy->GetStatComponent()->GetCurBoosting() < 50.f)
	{
		EnemyBehaviorComponent->SetServiceType(EServiceType::IDLE);
		EnemyBehaviorComponent->SetIdleTaskType(EIdleTaskType::STAT_CARE);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UC_BTTaskWait::OnWaitTimeFinished(UBehaviorTreeComponent& OwnerComp, AC_EnemyAIController* EnemyAIController, AC_Enemy* Enemy, UC_BehaviorComponent* EnemyBehaviorComponent)
{
	EnemyTimers[EnemyBehaviorComponent] = 0.f;

	// 만약 아직 비행기 이륙이 시작되지 않은 대기 시간 중이라면, MoveToRandomPos 처리만 걸어둠
	AC_AirplaneManager* AirplaneManager = GAMESCENE_MANAGER->GetAirplaneManager();
	
	if (AirplaneManager) if (!GAMESCENE_MANAGER->GetAirplaneManager()->GetHasAirplaneTakeOff())
	{
		ExecuteMoveToRandomLocation(Enemy, EnemyBehaviorComponent, false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	// MoveToRandomPos or Attack trial or 다시 Wait 중 택 1
	// Attack Trial 60% | MoveToRandomPos 20% | Wait 20%

	// 무조건 Attack Trial 시도 (ForTesting) TODO : 다시 돌려놓기
	EnemyAIController->TrySetTargetCharacterBasedOnPriority();
	if (IsValid(EnemyBehaviorComponent->GetTargetCharacter()))
	{
		// UC_Util::Print("WaitTask Time's up : Try Attack TargetCharacter!", FColor::Red, 10.f);
		EnemyBehaviorComponent->SetServiceType(EServiceType::COMBAT);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

	/*
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool bIsInSmokeArea = Enemy->GetSmokeEnteredChecker()->IsCurrentlyInSmokeArea();
	if (FMath::RandRange(0.f, 1.f) < 0.6f && !bIsInSmokeArea) // SetTargetCharacter & AttackTrial
	{
		EnemyAIController->TrySetTargetCharacterBasedOnPriority();
		if (IsValid(EnemyBehaviorComponent->GetTargetCharacter()))
		{
			// UC_Util::Print("WaitTask Time's up : Try Attack TargetCharacter!", FColor::Red, 10.f);
			EnemyBehaviorComponent->SetServiceType(EServiceType::COMBAT);
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (FMath::RandRange(0, 1)) // MoveToRandomPos
	{
		ExecuteMoveToRandomLocation(Enemy, EnemyBehaviorComponent, true);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// UC_Util::Print("WaitTask Time's up : Keep waiting...", FColor::Red, 10.f);
	EnemyBehaviorComponent->SetIdleTaskType(EIdleTaskType::WAIT); // 다시금 기다리기 처리(이 호출로 기다리는 총 시간 랜덤하게 다시 setting 됨)
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
}

void UC_BTTaskWait::ExecuteMoveToRandomLocation(AC_Enemy* Enemy, UC_BehaviorComponent* EnemyBehaviorComponent, bool bHasToMoveInsideMainCircle)
{
	if (bHasToMoveInsideMainCircle)	Enemy->GetTargetLocationSettingHelper()->SetRandomBasicTargetLocationInsideMainCircle(1000.f);
	else							Enemy->GetTargetLocationSettingHelper()->SetRandomTargetLocation(1000.f);
	
	EnemyBehaviorComponent->SetIdleTaskType(EIdleTaskType::BASIC_MOVETO);
}
