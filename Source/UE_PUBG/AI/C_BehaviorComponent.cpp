// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/C_BehaviorComponent.h"

#include "Character/C_BasicCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/C_Player.h"

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
	SetIdleTaskType(EIdleTaskType::BASIC_MOVETO);

	// TODO : TargetCharacter 다른 곳에서 잡아주는 로직 만들기
	SetTargetCharacter(GAMESCENE_MANAGER->GetPlayer());
	SetServiceType(EServiceType::COMBAT);
}

void UC_BehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UC_BehaviorComponent::SetTargetCharacter(AActor* InTargetCharacter)
{
	if (!IsValid(InTargetCharacter)) return false;
	Blackboard->SetValueAsObject(TargetCharacterKey, InTargetCharacter);
	return true;
}

class AC_BasicCharacter* UC_BehaviorComponent::GetTargetCharacter()
{
	return Cast<AC_BasicCharacter>(Blackboard->GetValueAsObject(TargetCharacterKey));
}

bool UC_BehaviorComponent::SetBehaviorType(EBehaviorType Type)
{
	if (Type == EBehaviorType::MAX) return false;

	Blackboard->SetValueAsEnum(BehaviorKey, (uint8)Type);
	return true;
}

bool UC_BehaviorComponent::SetServiceType(EServiceType Type)
{
	if (Type == EServiceType::MAX) return false;

	Blackboard->SetValueAsEnum(ServiceKey, (uint8)Type);
	return true;
}

bool UC_BehaviorComponent::SetIdleTaskType(EIdleTaskType Type)
{
	if (Type == EIdleTaskType::MAX) return false;

	Blackboard->SetValueAsEnum(IdleTaskKey, (uint8)Type);

	return false;
}



