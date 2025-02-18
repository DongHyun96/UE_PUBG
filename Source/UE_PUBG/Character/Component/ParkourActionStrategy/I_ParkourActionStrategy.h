// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "I_ParkourActionStrategy.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UI_ParkourActionStrategy : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UE_PUBG_API II_ParkourActionStrategy
{
	GENERATED_BODY()

	
public:

	/// <summary>
	/// Parkour Motion Warp action 실행
	/// </summary>
	/// <param name="TargetCharacter"></param>
	/// <param name="CurParkourDesc"></param>
	virtual void UseMotionWarpActionStrategy(class AC_BasicCharacter* TargetCharacter, const struct FParkourDescriptor& CurParkourDesc) = 0;

};


