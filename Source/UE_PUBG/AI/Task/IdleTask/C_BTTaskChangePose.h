// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "C_BTTaskChangePose.generated.h"

enum class EPoseState : uint8;

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BTTaskChangePose : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UC_BTTaskChangePose();

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:

	class AC_Enemy*				OwnerEnemy{};
	class UC_BehaviorComponent* OwnerBehaviorComponent{};
	
};
