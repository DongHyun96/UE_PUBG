// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "I_AIThrowableAttackStrategy.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UI_AIThrowableAttackStrategy : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UE_PUBG_API II_AIThrowableAttackStrategy
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	/// <summary>
	/// AIAttack 첫 실행 시작 시 호출될 AI 투척류 공격 전략 Interface 함수
	/// </summary>
	/// <param name="ThrowingWeapon"> : 이 전략 객체를 사용하는 투척류 객체 </param>
	/// <param name="InTargetCharacter"> : 공격 대상 Character </param>
	/// <returns> : 공격을 할 수 없는 상황이라면 return false </returns>
	virtual bool ExecuteAIAttack(class AC_ThrowingWeapon* ThrowingWeapon, class AC_BasicCharacter* InTargetCharacter) = 0;

	/// <summary>
	/// AIAttack TickTask에 호출될 AI 투척류 공격 전략 Interface 함수
	/// </summary>
	/// <param name="ThrowingWeapon"> : 이 전략 객체를 사용하는 투척류 객체 </param>
	/// <param name="InTargetCharacter"> : 공격 대상 Character </param>
	/// <returns> : TickTask처리가 끝났다면 return false (or TickTask처리가 필요하지 않다면 return false) </returns>
	virtual bool ExecuteAIAttackTickTask(class AC_ThrowingWeapon* ThrowingWeapon, class AC_BasicCharacter* InTargetCharacter, const float& DeltaTime) = 0;
	
};
