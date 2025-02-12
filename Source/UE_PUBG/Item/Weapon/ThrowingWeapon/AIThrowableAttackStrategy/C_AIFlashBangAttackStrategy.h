// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "I_AIThrowableAttackStrategy.h"
#include "UObject/NoExportTypes.h"
#include "C_AIFlashBangAttackStrategy.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_AIFlashBangAttackStrategy : public UObject, public II_AIThrowableAttackStrategy
{
	GENERATED_BODY()

// TODO : 얘는 cooking 없이 바로 던져야 할듯? Cooking 시간이 짧음
	
public:

	bool ExecuteAIAttack(AC_ThrowingWeapon* ThrowingWeapon, AC_BasicCharacter* InTargetCharacter) override;
	bool ExecuteAIAttackTickTask(AC_ThrowingWeapon* ThrowingWeapon, AC_BasicCharacter* InTargetCharacter, const float& DeltaTime) override;

	
};
