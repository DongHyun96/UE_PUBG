// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "I_ParkourActionStrategy.h"
#include "C_MantleLowActionStrategy.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_MantleLowActionStrategy : public UObject, public II_ParkourActionStrategy
{
	GENERATED_BODY()

private:

	void UseMotionWarpActionStrategy(AC_BasicCharacter* TargetCharacter, const FParkourDescriptor& CurParkourDesc) override;

};
