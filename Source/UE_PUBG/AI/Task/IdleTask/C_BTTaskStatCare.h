// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "C_BTTaskStatCare.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BTTaskStatCare : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UC_BTTaskStatCare();

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	/// <summary>
	/// Heal 아이템 사용 시도
	/// </summary>
	/// <returns> : 사용 시작 실패했다면 return false </returns>
	bool TryUsingHealItem(class AC_Enemy* Enemy, class UC_InvenComponent* EnemyInvenComponent);

	/// <summary>
	/// Boost 아이템 사용 시도 
	/// </summary>
	/// <returns> : 사용 시작 실패했다면 return false </returns>
	bool TryUsingBoostItem(AC_Enemy* Enemy, UC_InvenComponent* EnemyInvenComponent);

private:

	TMap<AC_Enemy*, class AC_ConsumableItem*> ExecutedConsumableItemMap{};
};
