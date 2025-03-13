// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/SkyDivingComponent/C_EnemySkyDivingComponent.h"

UC_EnemySkyDivingComponent::UC_EnemySkyDivingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_EnemySkyDivingComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UC_EnemySkyDivingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UC_EnemySkyDivingComponent::SetStateToSkyDivingState()
{
	// TODO : skyDiving EndPoint까지의 Velocity 잡아주기
	
}

void UC_EnemySkyDivingComponent::SetStateToParachutingState()
{
	// TODO : Target Location 까지의 Velocity 잡아주기
	
}

void UC_EnemySkyDivingComponent::SetStateToLandingState()
{
	
}
