// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/C_BehaviorComponent.h"

#include "EnhancedInputSubsystemInterface.h"
#include "Character/C_BasicCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/C_Player.h"
#include "Service/C_BTServiceCombat.h"

#include "Service/C_BTServiceIdle.h"
#include "Singleton/C_GameSceneManager.h"


UC_BehaviorComponent::UC_BehaviorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UC_BehaviorComponent::BeginPlay()
{
	Super::BeginPlay();

	SetServiceType(EServiceType::IDLE);
	SetIdleTaskType(EIdleTaskType::WAIT);

	// SetServiceType(EServiceType::COMBAT);
}

void UC_BehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UC_BehaviorComponent::SetServiceType(EServiceType Type)
{
	if (Type == EServiceType::MAX) return false;
	Blackboard->SetValueAsEnum(ServiceKey, static_cast<uint8>(Type));
	return true;
}

bool UC_BehaviorComponent::SetIdleTaskType(EIdleTaskType Type)
{
	if (Type == EIdleTaskType::MAX) return false;
	Blackboard->SetValueAsEnum(IdleTaskKey, static_cast<uint8>(Type));
	return false;
}

bool UC_BehaviorComponent::SetCombatTaskType(ECombatTaskType Type)
{
	if (Type == ECombatTaskType::MAX) return false;
	Blackboard->SetValueAsEnum(CombatTaskKey, static_cast<uint8>(Type));
	return false;
}

bool UC_BehaviorComponent::SetPlayer(class AC_Player* Player)
{
	if (!IsValid(Player)) return false;
	Blackboard->SetValueAsObject(PlayerKey, Player);
	return true;
}

bool UC_BehaviorComponent::SetTargetCharacter(AActor* InTargetCharacter)
{
	if (!IsValid(InTargetCharacter)) return false;
	Blackboard->SetValueAsObject(TargetCharacterKey, InTargetCharacter);
	return true;
}

bool UC_BehaviorComponent::SetTargetLocation(const FVector& InTargetLocation)
{
	Blackboard->SetValueAsVector(TargetLocationKey, InTargetLocation);
	return true;
}

FVector UC_BehaviorComponent::GetTargetLocation() const
{
	return Blackboard->GetValueAsVector(TargetLocationKey);
}

class AC_BasicCharacter* UC_BehaviorComponent::GetTargetCharacter() const
{
	return Cast<AC_BasicCharacter>(Blackboard->GetValueAsObject(TargetCharacterKey));
}


