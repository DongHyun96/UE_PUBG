// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "C_BTServiceStateMachine.generated.h"

/**
 * Currently not in used (BTTaskWait에서 기본 FSM 처리 중)
 */
UCLASS()
class UE_PUBG_API UC_BTServiceStateMachine : public UBTService
{
	GENERATED_BODY()

public:

	UC_BTServiceStateMachine();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	
	/// <summary>
	/// Random한 Target 지점 setting 후 이동 
	/// </summary>
	/// <param name="Enemy"></param>
	/// <param name="EnemyBehaviorComponent"></param>
	/// <param name="bMoveRandomlyInsideMainCircle"> : Random한 지점이 MainCircle 내부의 지점이어야 하는지 체크 </param>
	void ExecuteMoveToRandomLocation(class AC_Enemy* Enemy, class UC_BehaviorComponent* EnemyBehaviorComponent, bool bMoveRandomlyInsideMainCircle);

private:

	TMap<class UC_BehaviorComponent*, float> EnemyTimers{};

};


