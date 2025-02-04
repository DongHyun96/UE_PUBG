// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "C_BTTaskSkyDive.generated.h"

/**
 * 
 */


/*
TODO :
SkyDive할 목표지점 설정(땅)
최단 거리가 되었을 때 비행기에서 내리기
비행기에서 내린 이후 목표 지점으로 SkyDive 진행
*/

UCLASS()
class UE_PUBG_API UC_BTTaskSkyDive : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UC_BTTaskSkyDive();

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	
	class AC_Enemy* OwnerEnemy{};
};
