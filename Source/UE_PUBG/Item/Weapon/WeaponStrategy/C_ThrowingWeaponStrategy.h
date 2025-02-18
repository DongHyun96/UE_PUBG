// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "I_WeaponButtonStrategy.h"
#include "C_ThrowingWeaponStrategy.generated.h"

/**
 * 
 */
//UCLASS(Abstract)
UCLASS()
class UE_PUBG_API AC_ThrowingWeaponStrategy : public AActor, public II_WeaponButtonStrategy
{
	GENERATED_BODY()

public:

	bool UseBKeyStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)			override;

	bool UseRKeyStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)			override;

public:

	bool UseMlb_StartedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)	override;
	bool UseMlb_OnGoingStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)	override;
	bool UseMlb_CompletedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)	override;

	bool UseMrb_StartedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)	override;
	bool UseMrb_OnGoingStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)	override;
	bool UseMrb_CompletedStrategy(AC_BasicCharacter* WeaponUser, AC_Weapon* Weapon)	override;

};


