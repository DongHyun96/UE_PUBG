// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Vehicle.h"

#include "Components/BoxComponent.h"
#include "Utility/C_Util.h"

AC_Vehicle::AC_Vehicle()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_Vehicle::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsValid(MainOuterBoxComponent)) UC_Util::Print("From AC_Vehicle::BeginPlay : MainOuterBoxComponent not valid", FColor::Red, 10.f);
}

void AC_Vehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

