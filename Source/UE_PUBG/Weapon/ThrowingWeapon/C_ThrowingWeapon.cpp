// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ThrowingWeapon/C_ThrowingWeapon.h"

AC_ThrowingWeapon::AC_ThrowingWeapon()
{
}

void AC_ThrowingWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AC_ThrowingWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AC_ThrowingWeapon::AttachToHolster(USceneComponent* InParent)
{
	return false;
}

bool AC_ThrowingWeapon::AttachToHand(USceneComponent* InParent)
{
	return false;
}
