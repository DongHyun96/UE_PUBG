// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/MeleeWeapon/C_MeleeWeapon.h"
#include "Weapon/WeaponStrategy/I_WeaponButtonStrategy.h"
#include "Weapon/WeaponStrategy/C_MeleeWeaponStrategy.h"


AC_MeleeWeapon::AC_MeleeWeapon()
{
	WeaponButtonStrategy = NewObject<UC_MeleeWeaponStrategy>();
}

void AC_MeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AC_MeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
