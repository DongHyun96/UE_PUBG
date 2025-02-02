// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "C_BTServiceSkyDive.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BTServiceSkyDive : public UBTService
{
	GENERATED_BODY()

public:

	UC_BTServiceSkyDive();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
