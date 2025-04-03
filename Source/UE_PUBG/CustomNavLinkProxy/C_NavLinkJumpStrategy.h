// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "I_NavLinkProxyActionStrategy.h"
#include "UObject/NoExportTypes.h"
#include "C_NavLinkJumpStrategy.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_NavLinkJumpStrategy : public UObject, public II_NavLinkProxyActionStrategy
{
	GENERATED_BODY()

private:
	bool ExecuteStartPointAction(AC_CustomNavLinkProxy* CustomNavLinkProxy, AC_Enemy* EnemyAgent, const FVector& StartPoint, const EDirection& Direction) override;
};
