// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/C_BehaviorComponent.h"

#include "EnhancedInputSubsystemInterface.h"
#include "Character/C_BasicCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/C_Player.h"
#include "Service/C_BTServiceCombat.h"

#include "Service/C_BTServiceIdle.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"


UC_BehaviorComponent::UC_BehaviorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UC_BehaviorComponent::BeginPlay()
{
	Super::BeginPlay();

	SetServiceType(EServiceType::IDLE);
	// SetServiceType(EServiceType::COMBAT);

	SetIdleTaskType(EIdleTaskType::BASIC_MOVETO);
}

void UC_BehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*switch (static_cast<EIdleTaskType>(Blackboard->GetValueAsEnum(IdleTaskKey))) {
	case EIdleTaskType::WAIT: UC_Util::Print("Wait"); break;
	case EIdleTaskType::BASIC_MOVETO: UC_Util::Print("Move To"); break;
	case EIdleTaskType::CHANGE_POSE: UC_Util::Print("Change Pose"); break;
	case EIdleTaskType::MAX: UC_Util::Print("Max"); break;
	}*/
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

	if (Type == EIdleTaskType::CHANGE_POSE)
	{
		// 이전 Type 저장하고 ChangePose 끝날 때 다시 돌아가기
		PrevIdleTaskType = static_cast<EIdleTaskType>(Blackboard->GetValueAsEnum(IdleTaskKey));
	}
	
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

void UC_BehaviorComponent::SetNextPoseState(EPoseState InNextPoseState)
{
	UC_Util::Print("SetNextPoseState", GAMESCENE_MANAGER->GetTickRandomColor(), 10.f);
	NextPoseState = InNextPoseState;
}

bool UC_BehaviorComponent::SetIdleTaskTypeToPrevType()
{
	if (PrevIdleTaskType == EIdleTaskType::MAX) return false;
	EIdleTaskType CurrentType = static_cast<EIdleTaskType>(Blackboard->GetValueAsEnum(IdleTaskKey));
	if (CurrentType == PrevIdleTaskType)
	{
		UC_Util::Print("From UC_BehaviorComponent::SetIdleTaskTypeToPrevType : Prev Type is same with current type!", FColor::Red, 10.f);
		return false;
	}
	Blackboard->SetValueAsEnum(IdleTaskKey, static_cast<uint8>(PrevIdleTaskType));
	PrevIdleTaskType = EIdleTaskType::MAX;
	return true;
}

class AC_BasicCharacter* UC_BehaviorComponent::GetTargetCharacter() const
{
	return Cast<AC_BasicCharacter>(Blackboard->GetValueAsObject(TargetCharacterKey));
}


