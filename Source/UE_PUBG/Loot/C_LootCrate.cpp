// Fill out your copyright notice in the Description page of Project Settings.


#include "Loot/C_LootCrate.h"

// Sets default values
AC_LootCrate::AC_LootCrate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//SetActorEnableCollision(false);
}

// Called when the game starts or when spawned
void AC_LootCrate::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AC_LootCrate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

