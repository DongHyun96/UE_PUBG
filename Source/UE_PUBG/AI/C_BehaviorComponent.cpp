// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/C_BehaviorComponent.h"

#include "Character/C_BasicCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"


UC_BehaviorComponent::UC_BehaviorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UC_BehaviorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UC_BehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UC_BehaviorComponent::SetPlayer(AActor* InPlayer)
{
	Blackboard->SetValueAsObject(PlayerKey, InPlayer);
}

AC_BasicCharacter* UC_BehaviorComponent::GetPlayer()
{
	return Cast<AC_BasicCharacter>(Blackboard->GetValueAsObject(PlayerKey));
}

bool UC_BehaviorComponent::ChangeType(EBehaviorType Type)
{
	if (Type == EBehaviorType::MAX) return false;

	Blackboard->SetValueAsEnum(BehaviorKey, (uint8)Type);
	return true;
}

bool UC_BehaviorComponent::ChangeServiceType(EServiceType Type)
{
	if (Type == EServiceType::MAX) return false;

	Blackboard->SetValueAsEnum(ServiceKey, (uint8)Type);
	return true;
}

