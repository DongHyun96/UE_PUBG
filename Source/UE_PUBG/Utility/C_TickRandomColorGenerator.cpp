// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/C_TickRandomColorGenerator.h"

AC_TickRandomColorGenerator::AC_TickRandomColorGenerator()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AC_TickRandomColorGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

void AC_TickRandomColorGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickRandomColor = FColor::MakeRandomColor();

}

