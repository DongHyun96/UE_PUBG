// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "C_BTTaskAttack.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BTTaskAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UC_BTTaskAttack();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:

	// 세팅된 Attack weapons
	TMap<class AC_Enemy*, class AC_Weapon*> AttackingWeapons{};
	
};
