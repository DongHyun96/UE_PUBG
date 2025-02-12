// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/ThrowingWeapon/AIThrowableAttackStrategy/C_AIFlashBangAttackStrategy.h"

#include "Utility/C_Util.h"

bool UC_AIFlashBangAttackStrategy::ExecuteAIAttack(AC_ThrowingWeapon* ThrowingWeapon, AC_BasicCharacter* InTargetCharacter)
{
	UC_Util::Print("Try Executing FlashBang AI Attack Strategy", FColor::Red, 10.f);

	// TODO : TargetCharacter 위치로부터 적정 Offset주어 던질 Projectile destination 지정
	// SuggestTossVelocity 통해 Velocity 정하기
	
	return false;
}

bool UC_AIFlashBangAttackStrategy::ExecuteAIAttackTickTask(AC_ThrowingWeapon* ThrowingWeapon, AC_BasicCharacter* InTargetCharacter, const float& DeltaTime)
{
	UC_Util::Print("Try Executing FlashBang AI Attack TickTask Strategy", FColor::Red, 10.f);
	
	return false;
}
