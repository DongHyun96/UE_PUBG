// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/ThrowingWeapon/C_GrenadeExplode.h"
#include "Utility/C_Util.h"
bool AC_GrenadeExplode::UseStrategy(AC_ThrowingWeapon* ThrowingWeapon)
{
	// TODO : 폭파 vfx, sfx / 
	// 폭발 반경 내에 캐릭터가 존재한다면 ray cast -> ray cast hit -> damage 주기
	// 반경 내에 캐릭터가 존재하면 aim punching & 플레이어의 경우 camera shake
	
	UC_Util::Print("Grenade Explode");
	return true;
}

