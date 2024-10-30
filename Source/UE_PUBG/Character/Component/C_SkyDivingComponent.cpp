// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_SkyDivingComponent.h"

UC_SkyDivingComponent::UC_SkyDivingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UC_SkyDivingComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UC_SkyDivingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

