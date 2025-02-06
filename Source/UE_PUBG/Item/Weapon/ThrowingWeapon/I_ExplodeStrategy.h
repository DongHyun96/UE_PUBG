// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "I_ExplodeStrategy.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UI_ExplodeStrategy : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UE_PUBG_API II_ExplodeStrategy
{
	GENERATED_BODY()

public:

	/// <summary>
	/// THrowing Weapon 터지는 전략 abstract method | 터치고 Destroy object는 주인 쪽에서
	/// </summary>
	/// <param name="ThrowingWeapon"> : 터질 Throwing Weapon </param>
	/// <returns> : 터치기 성공했다면 return true </returns>
	virtual bool UseStrategy(class AC_ThrowingWeapon* ThrowingWeapon) = 0;
};


