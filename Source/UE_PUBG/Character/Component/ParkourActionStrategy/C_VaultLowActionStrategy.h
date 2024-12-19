// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "I_ParkourActionStrategy.h"
#include "C_VaultLowActionStrategy.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_VaultLowActionStrategy : public UObject, public II_ParkourActionStrategy
{
	GENERATED_BODY()

public:
	
	void UseMotionWarpActionStrategy(AC_BasicCharacter* TargetCharacter, const FParkourDescriptor& CurParkourDesc) override;

};
