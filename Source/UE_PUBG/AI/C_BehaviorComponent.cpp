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
	// OwnerEnemyAIController->SetFocus(GAMESCENE_MANAGER->GetPlayer());
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

	// FlashBang 피격 상태 중이라면
	if (OwnerEnemyAIController->IsFlashBangEffectTimeLeft()) return false;

	Blackboard->SetValueAsEnum(ServiceKey, static_cast<uint8>(Type));
	return true;
}

EServiceType UC_BehaviorComponent::GetServiceType() const
{
	return static_cast<EServiceType>(Blackboard->GetValueAsEnum(ServiceKey));
}

void UC_BehaviorComponent::Dead()
{
	Blackboard->SetValueAsBool(IsDeadKey, true);
}

bool UC_BehaviorComponent::SetIdleTaskType(EIdleTaskType Type)
{
	if (Type == EIdleTaskType::MAX) return false;
	if (OwnerEnemyAIController->IsFlashBangEffectTimeLeft()) return false;

	// Random wait time 지정
	if (Type == EIdleTaskType::WAIT) WaitTime = FMath::RandRange(5.f, 10.f);
	
	Blackboard->SetValueAsEnum(IdleTaskKey, static_cast<uint8>(Type));
	return false;
}

EIdleTaskType UC_BehaviorComponent::GetIdleTaskType() const
{
	return static_cast<EIdleTaskType>(Blackboard->GetValueAsEnum(IdleTaskKey));
}

void UC_BehaviorComponent::OnTargetCharacterDead(class AC_BasicCharacter* DeadCharacter)
{
	if (GetTargetCharacter() != DeadCharacter)
	{
		UC_Util::Print("From UC_BehaviorComponent::OnTargetCharacterDead : "
				 "Received Dead delegate callback, but TargetCharacter mismatched with dead character!", FColor::Red, 10.f);
		return;
	}
	SetTargetCharacter(nullptr);
	OwnerEnemyAIController->UpdateDetectedCharactersRangeLevel();
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
	if (GetServiceType() == EServiceType::COMBAT) return false;
	
	// 새로운 TargetCharacter로 세팅 이전, 이전 TargetCharacter OnDead delegate 구독 끊기
	if (AC_BasicCharacter* PrevTargetCharacter = GetTargetCharacter())
		PrevTargetCharacter->Delegate_OnCharacterDead.RemoveAll(this);
	
	Blackboard->SetValueAsObject(TargetCharacterKey, InTargetCharacter);
	
	InTargetCharacter->Delegate_OnCharacterDead.AddUObject(this, &UC_BehaviorComponent::OnTargetCharacterDead);
	
	OwnerEnemy->SetTargetCharacterWidgetName(InTargetCharacter->GetName()); // TODO : 이 라인 지우기(For Testing)

	
	return true;
}

bool UC_BehaviorComponent::SetBasicTargetLocation(const FVector& InTargetLocation)
{
	Blackboard->SetValueAsVector(BasicTargetLocationKey, InTargetLocation);

	// 만약에 현재 이동 중이라면, 새로운 이동 목표지점 바로 setting
	if (GetServiceType() == EServiceType::IDLE && GetIdleTaskType() == EIdleTaskType::BASIC_MOVETO)
		OwnerEnemyAIController->MoveToLocation(InTargetLocation);
	
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
		OwnerEnemyAIController->MoveToLocation(InTargetLocation);
	
	return true;
}

FVector UC_BehaviorComponent::GetInCircleTargetLocation() const
{
	return Blackboard->GetValueAsVector(InCircleTargetLocationKey);
}

class AC_BasicCharacter* UC_BehaviorComponent::GetTargetCharacter() const
{
	return Cast<AC_BasicCharacter>(Blackboard->GetValueAsObject(TargetCharacterKey));
}


