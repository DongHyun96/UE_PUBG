// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "I_WeaponButtonStrategy.h"
#include "C_GunStrategy.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_GunStrategy : public UObject, public II_WeaponButtonStrategy
{
	GENERATED_BODY()
	
public:

	bool UseBKeyStrategy(C_BasicCharacter* WeaponUser, AC_Weapon* Weapon)			override;

	bool UseRKeyStrategy(C_BasicCharacter* WeaponUser, AC_Weapon* Weapon)			override;

public:

	bool UseMlb_StartedStrategy(C_BasicCharacter* WeaponUser, AC_Weapon* Weapon)	override;
	bool UseMlb_OnGoingStrategy(C_BasicCharacter* WeaponUser, AC_Weapon* Weapon)	override;
	bool UseMlb_CompletedStrategy(C_BasicCharacter* WeaponUser, AC_Weapon* Weapon)	override;

	bool UseMrb_StartedStrategy(C_BasicCharacter* WeaponUser, AC_Weapon* Weapon)	override;
	bool UseMrb_OnGoingStrategy(C_BasicCharacter* WeaponUser, AC_Weapon* Weapon)	override;
	bool UseMrb_CompletedStrategy(C_BasicCharacter* WeaponUser, AC_Weapon* Weapon)	override;

};
