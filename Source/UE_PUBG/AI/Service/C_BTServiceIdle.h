// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "C_BTServiceIdle.generated.h"

UENUM(BlueprintType)
enum class EIdleTaskType : uint8
{
	WAIT,
	BASIC_MOVETO,
	CHANGE_POSE,
	MAX
};

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BTServiceIdle : public UBTService
{
	GENERATED_BODY()

public:

	UC_BTServiceIdle();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
