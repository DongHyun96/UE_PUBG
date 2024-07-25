// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/ThrowingWeapon/C_SmokeGrndExplode.h"
#include "Utility/C_Util.h"

bool AC_SmokeGrndExplode::UseStrategy(AC_ThrowingWeapon* ThrowingWeapon)
{
	UC_Util::Print("Smoke Grenade Explode!");
	return true;
}
