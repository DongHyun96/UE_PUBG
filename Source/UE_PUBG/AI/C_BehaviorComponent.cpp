// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/C_BehaviorComponent.h"

#include "C_EnemyAIController.h"
#include "EnhancedInputSubsystemInterface.h"
#include "Character/C_BasicCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
#include "Navigation/PathFollowingComponent.h"
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
	SetIdleTaskType(EIdleTaskType::WAIT);
	// SetIdleTaskType(EIdleTaskType::BASIC_MOVETO);
	SetTargetCharacter(GAMESCENE_MANAGER->GetPlayer());
}

void UC_BehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DrawDebugSphere(GetWorld(), GetBasicTargetLocation(), 15.f, 10, FColor::Red);

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

EServiceType UC_BehaviorComponent::GetServiceType() const
{
	return static_cast<EServiceType>(Blackboard->GetValueAsEnum(ServiceKey));
}

bool UC_BehaviorComponent::SetIdleTaskType(EIdleTaskType Type)
{
	if (Type == EIdleTaskType::MAX) return false;

	if (Type == EIdleTaskType::CHANGE_POSE)
	{
		// 이전 Type 저장하고 ChangePose 끝날 때 다시 돌아가기
		PrevIdleTaskType = static_cast<EIdleTaskType>(Blackboard->GetValueAsEnum(IdleTaskKey));
	}

	// Random wait time 지정
	if (Type == EIdleTaskType::WAIT) WaitTime = FMath::RandRange(5.f, 10.f);
	
	Blackboard->SetValueAsEnum(IdleTaskKey, static_cast<uint8>(Type));
	return false;
}

EIdleTaskType UC_BehaviorComponent::GetIdleTaskType() const
{
	return static_cast<EIdleTaskType>(Blackboard->GetValueAsEnum(IdleTaskKey));
}

bool UC_BehaviorComponent::SetCombatTaskType(ECombatTaskType Type)
{
	if (Type == ECombatTaskType::MAX) return false;
	Blackboard->SetValueAsEnum(CombatTaskKey, static_cast<uint8>(Type));
	return false;
}

ECombatTaskType UC_BehaviorComponent::GetCombatTaskType() const
{
	return static_cast<ECombatTaskType>(Blackboard->GetValueAsEnum(CombatTaskKey));
}

bool UC_BehaviorComponent::SetPlayer(class AC_Player* Player)
{
	if (!IsValid(Player)) return false;
	Blackboard->SetValueAsObject(PlayerKey, Player);
	return true;
}

void UC_BehaviorComponent::OnTargetCharacterDead(class AC_BasicCharacter* DeadCharacter)
{
	if (GetTargetCharacter() != DeadCharacter)
	{
		UC_Util::Print("From UC_BehaviorComponent::OnTargetCharacterDead : "
				 "Received Dead delegate callback, but TargetCharacter mismatched with dead character!", FColor::Red, 10.f);
		return;
	}

	// TODO : Sight에 이미 들어와 있는 캐릭터 중 Random한 캐릭터 지정해서 새로운 TargetCharacter 지정
	// 만약에 없다면 nullptr로 지정해주기

	SetTargetCharacter(nullptr);
}

bool UC_BehaviorComponent::SetTargetCharacter(AC_BasicCharacter* InTargetCharacter)
{
	if (!IsValid(InTargetCharacter))
	{
		// Valid하지 않은 TargetCharacter의 경우, TargetCharacter를 놓쳤다고 판단(TargetCharacter의 사망 등)
		Blackboard->SetValueAsObject(TargetCharacterKey, nullptr);
		OwnerEnemy->SetTargetCharacterWidgetName("NONE");
		return false;
	}

	// TargetCharacter가 이미 죽었을 때
	if (InTargetCharacter->GetMainState() == EMainState::DEAD) return false;

	// 현재 공격중인 때에는 TargetCharacter를 새로 바꾸지 않음
	if (GetServiceType() == EServiceType::COMBAT && GetCombatTaskType() == ECombatTaskType::ATTACK) return false;
	
	
	Blackboard->SetValueAsObject(TargetCharacterKey, InTargetCharacter);

	OwnerEnemy->SetTargetCharacterWidgetName(InTargetCharacter->GetName()); // TODO : 이 라인 지우기(For Testing)
	
	return true;
}

bool UC_BehaviorComponent::SetBasicTargetLocation(const FVector& InTargetLocation)
{
	Blackboard->SetValueAsVector(BasicTargetLocationKey, InTargetLocation);

	// 만약에 현재 이동 중이라면, 새로운 이동 목표지점 바로 setting
	if (GetServiceType() == EServiceType::IDLE && GetIdleTaskType() == EIdleTaskType::BASIC_MOVETO)
		OwnerEnemy->GetEnemyAIController()->MoveToLocation(InTargetLocation);
	
	return true;
}

FVector UC_BehaviorComponent::GetBasicTargetLocation() const
{
	return Blackboard->GetValueAsVector(BasicTargetLocationKey);
}

bool UC_BehaviorComponent::SetInCircleTargetLocation(const FVector& InTargetLocation)
{
	Blackboard->SetValueAsVector(InCircleTargetLocationKey, InTargetLocation);

	// 만약에 현재 이동 중이라면, 새로운 이동 목표지점 바로 setting
	if (GetServiceType() == EServiceType::IDLE && GetIdleTaskType() == EIdleTaskType::INCIRCLE_MOVETO)
		OwnerEnemy->GetEnemyAIController()->MoveToLocation(InTargetLocation);
	
	return true;
}

FVector UC_BehaviorComponent::GetInCircleTargetLocation() const
{
	return Blackboard->GetValueAsVector(InCircleTargetLocationKey);
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


