// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/C_Item.h"


#include "Weapon/WeaponStrategy/I_WeaponButtonStrategy.h"
#include "Weapon/WeaponStrategy/C_GunStrategy.h"

// Sets default values
AC_Item::AC_Item()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    //WeaponButtonStrategy = CreateDefaultSubobject<AC_GunStrategy>("GunStrategy");

}

// Called when the game starts or when spawned
void AC_Item::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AC_Item::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AttachToSocket(USceneComponent* InParent)
{

}
