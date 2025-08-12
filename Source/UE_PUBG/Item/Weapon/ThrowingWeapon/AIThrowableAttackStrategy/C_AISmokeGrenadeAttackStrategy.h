// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "I_AIThrowableAttackStrategy.h"
#include "UObject/NoExportTypes.h"
#include "C_AISmokeGrenadeAttackStrategy.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_AISmokeGrenadeAttackStrategy : public UObject, public II_AIThrowableAttackStrategy
{
	GENERATED_BODY()

public:

	// 쿠킹하고 제자리 던지기
	
	bool ExecuteAIAttack(AC_ThrowingWeapon* ThrowingWeapon, AC_BasicCharacter* InTargetCharacter) override;
	bool ExecuteAIAttackTickTask(AC_ThrowingWeapon* ThrowingWeapon, AC_BasicCharacter* InTargetCharacter, const float& DeltaTime) override;

private:

	TMap<AC_ThrowingWeapon*, float> CookingStartTimers{};
	// float CookingStartTimer{};
	
	static const float TIME_TO_COOK_AND_RELEASE;
	
};
