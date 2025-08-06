// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "C_BTTaskMoveTo.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BTTaskMoveTo : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:

	UC_BTTaskMoveTo();

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
	
	/// <summary>
	/// 현재 OwnerEnemy가 NavMesh 위에 있는지 조사 
	/// </summary>
	bool IsAgentOnNavMesh(AActor* Agent);

private:

	// NavMesh를 벗어나서 새로이 근처의 Nav Mesh로 이동중인지 파악
	// bool bHasMoveToNearestNavMeshStarted{};

private:
	
	class UNavigationSystemV1* NavSystem{};
	
};
