// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/C_Gun.h"

#include "Weapon/WeaponStrategy/I_WeaponButtonStrategy.h"
#include "Weapon/WeaponStrategy/C_GunStrategy.h"

// Sets default values
AC_Gun::AC_Gun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    //WeaponButtonStrategy = CreateDefaultSubobject<AC_GunStrategy>("GunStrategy");

}

bool AC_Gun::AttachToHolster(USceneComponent* InParent)
{
    return false;
}

bool AC_Gun::AttachToHand(USceneComponent* InParent)
{
    return false;
}


