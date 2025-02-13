// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "I_AIThrowableAttackStrategy.h"
#include "UObject/NoExportTypes.h"
#include "C_AIThrowingStrategy.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_AIThrowingStrategy : public UObject, public II_AIThrowableAttackStrategy
{
	GENERATED_BODY()
	
public:
	bool ExecuteAIAttack(AC_ThrowingWeapon* ThrowingWeapon, AC_BasicCharacter* InTargetCharacter) override;
	bool ExecuteAIAttackTickTask(AC_ThrowingWeapon* ThrowingWeapon, AC_BasicCharacter* InTargetCharacter, const float& DeltaTime) override;

private:
	/// <summary>
	/// 투척류의 ProjectileLaunchValues 세팅하기
	/// </summary>
	/// <param name="ThrowingWeapon"></param>
	/// <param name="TargetCharacterLocation"></param>
	/// <returns> : Setting 될 수 없는 환경이라면 return false </returns>
	bool UpdateProjectileLaunchValues(class AC_ThrowingWeapon* ThrowingWeapon, const FVector& TargetCharacterLocation);

	/// <summary>
	/// 던질 수 있는 제한 거리 이내에 있는지 check
	/// </summary>
	/// <param name="ThrowingWeaponLocation"></param>
	/// <param name="TargetCharacterLocation"></param>
	/// <returns></returns>
	bool IsPossibleDistanceToThrow(const FVector& ThrowingWeaponLocation, const FVector& TargetCharacterLocation);

	/// <summary>
	/// TargetCharacter위치로부터 적정 Random Position Get
	/// </summary>
	/// <param name="TargetCharacterLocation"></param>
	/// <returns></returns>
	FVector GetRandomNearByPositionFromTargetCharacter(const FVector& TargetCharacterLocation);
	

private:
	static const float TIME_TO_THROW;
	float ThrowTimer{};
	
};
