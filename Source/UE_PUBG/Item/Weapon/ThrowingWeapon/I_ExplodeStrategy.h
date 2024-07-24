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
	/// THrowing Weapon ������ ���� abstract method | ��ġ�� Destroy object�� ���� �ʿ���
	/// </summary>
	/// <param name="ThrowingWeapon"> : ���� Throwing Weapon </param>
	/// <returns> : ��ġ�� �����ߴٸ� return true </returns>
	virtual bool UseStrategy(class AC_ThrowingWeapon* ThrowingWeapon) = 0;
};
