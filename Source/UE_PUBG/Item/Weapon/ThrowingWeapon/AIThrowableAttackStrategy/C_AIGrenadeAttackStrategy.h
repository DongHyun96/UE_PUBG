// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "I_AIThrowableAttackStrategy.h"
#include "UObject/NoExportTypes.h"
#include "C_AIGrenadeAttackStrategy.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_AIGrenadeAttackStrategy : public UObject, public II_AIThrowableAttackStrategy
{
	GENERATED_BODY()
	
public:
	bool ExecuteAIAttack(AC_ThrowingWeapon* ThrowingWeapon, AC_BasicCharacter* InTargetCharacter) override;
	bool ExecuteAIAttackTickTask(AC_ThrowingWeapon* ThrowingWeapon, AC_BasicCharacter* InTargetCharacter, const float& DeltaTime) override;

};
