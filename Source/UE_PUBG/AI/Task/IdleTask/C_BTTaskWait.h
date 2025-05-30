// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "C_BTTaskWait.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BTTaskWait : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UC_BTTaskWait();

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	
	/// <summary>
	/// <para> WaitTime이 남아있을 때 처리될 내용 </para>
	/// <para> 아직 WaitTime이 모두 끝나지 않았을 때의 State handling </para>
	/// </summary>
	void OnWaitTimeRemain(UBehaviorTreeComponent& OwnerComp, class AC_EnemyAIController* EnemyAIController, class AC_Enemy* Enemy, class UC_BehaviorComponent* EnemyBehaviorComponent);

	/// <summary>
	/// WaitTime이 모두 끝났을 때 처리될 내용
	/// </summary>
	void OnWaitTimeFinished(UBehaviorTreeComponent& OwnerComp, AC_EnemyAIController* EnemyAIController, AC_Enemy* Enemy, UC_BehaviorComponent* EnemyBehaviorComponent);

private:
	
	/// <summary>
	/// Random한 Target 지점 setting 후 이동 
	/// </summary>
	/// <param name="Enemy"></param>
	/// <param name="EnemyBehaviorComponent"></param>
	/// <param name="bHasToMoveInsideMainCircle"> : Random한 지점이 MainCircle 내부의 지점이어야 하는지 체크 </param>
	void ExecuteMoveToRandomLocation(class AC_Enemy* Enemy, class UC_BehaviorComponent* EnemyBehaviorComponent, bool bHasToMoveInsideMainCircle);

private:

	TMap<class UC_BehaviorComponent*, float> EnemyTimers{};
	
};
