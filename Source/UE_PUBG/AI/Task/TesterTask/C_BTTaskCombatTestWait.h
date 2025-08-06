// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Task/IdleTask/C_BTTaskWait.h"
#include "C_BTTaskCombatTestWait.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BTTaskCombatTestWait : public UC_BTTaskWait
{
	GENERATED_BODY()

private:

	void OnWaitTimeRemain(UBehaviorTreeComponent& OwnerComp, AC_EnemyAIController* EnemyAIController,
		AC_Enemy* Enemy, UC_BehaviorComponent* EnemyBehaviorComponent) override;
	
	void OnWaitTimeFinished(UBehaviorTreeComponent& OwnerComp, AC_EnemyAIController* EnemyAIController,
		AC_Enemy* Enemy, UC_BehaviorComponent* EnemyBehaviorComponent) override;

};
