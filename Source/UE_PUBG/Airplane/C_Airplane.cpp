// Fill out your copyright notice in the Description page of Project Settings.


#include "Airplane/C_Airplane.h"

// Sets default values
AC_Airplane::AC_Airplane()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AC_Airplane::BeginPlay()
{
	Super::BeginPlay();

	this->SetActorHiddenInGame(true);
}

// Called every frame
void AC_Airplane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

