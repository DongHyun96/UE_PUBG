// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/ThrowingWeapon/C_GrenadeExplode.h"
#include "Utility/C_Util.h"
bool AC_GrenadeExplode::UseStrategy(AC_ThrowingWeapon* ThrowingWeapon)
{
	// TODO : ���� vfx, sfx / 
	// ���� �ݰ� ���� ĳ���Ͱ� �����Ѵٸ� ray cast -> ray cast hit -> damage �ֱ�
	// �ݰ� ���� ĳ���Ͱ� �����ϸ� aim punching & �÷��̾��� ��� camera shake
	
	UC_Util::Print("Grenade Explode");
	return true;
}

