// Fill out your copyright notice in the Description page of Project Settings.


#include "Loot/C_BasicLoot.h"

// Sets default values
AC_BasicLoot::AC_BasicLoot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AC_BasicLoot::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AC_BasicLoot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

