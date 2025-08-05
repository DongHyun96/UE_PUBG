// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/C_BehaviorComponent.h"

#include "C_EnemyAIController.h"
#include "Character/C_BasicCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/C_Enemy.h"
#include "Character/C_Player.h"

#include "Service/C_BTServiceIdle.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"


const FName UC_BehaviorComponent::PlayerKey = "Player";
const FName UC_BehaviorComponent::ServiceKey = "Service";

const FName UC_BehaviorComponent::TargetCharacterKey = "TargetCharacter"; // 현재 교전중인 Character

const FName UC_BehaviorComponent::BasicTargetLocationKey    = "BasicTargetLocation";
const FName UC_BehaviorComponent::InCircleTargetLocationKey = "InCircleTargetLocation";

const FName UC_BehaviorComponent::IdleTaskKey   = "IdleTask";
const FName UC_BehaviorComponent::CombatTaskKey = "CombatTask";

UC_BehaviorComponent::UC_BehaviorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UC_BehaviorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!OwnerEnemy->GetBehaviorTree()) OwnerEnemy->InitBehaviorTreeBySelfBehaviorType();
		
	SetServiceType(EServiceType::IDLE);
	// SetServiceType(EServiceType::COMBAT);
	SetIdleTaskType(EIdleTaskType::WAIT);
	// SetIdleTaskType(EIdleTaskType::BASIC_MOVETO);
	// SetTargetCharacter(GAMESCENE_MANAGER->GetPlayer());
	// OwnerEnemyAIController->SetFocus(GAMESCENE_MANAGER->GetPlayer());

	// OwnerEnemy가 Stat-Care 전용 Enemy인 경우, 항상 TargetCharacter를 Player로 둠 
	if (OwnerEnemy->GetBehaviorType() == EEnemyBehaviorType::StatCareTest)
		SetTargetCharacter(GAMESCENE_MANAGER->GetPlayer());

	// SkyDivingTester의 경우, 첫 자리를 TargetLocation으로 고수
	if (OwnerEnemy->GetBehaviorType() == EEnemyBehaviorType::SkyDivingTest)
		SetBasicTargetLocation(OwnerEnemy->GetActorLocation());
}

void UC_BehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// DrawDebugSphere(GetWorld(), GetBasicTargetLocation(), 15.f, 10, FColor::Red);
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

bool UC_BehaviorComponent::SetIdleTaskType(EIdleTaskType Type)
{
	if (Type == EIdleTaskType::MAX) return false;
	if (OwnerEnemyAIController->IsFlashBangEffectTimeLeft()) return false;

	// Random wait time 지정
	if (Type == EIdleTaskType::WAIT)
	{
		float RandBias = FMath::Pow(FMath::FRand(), 2.0f); // 0 ~ 1 -> 0에 더 가까운 값으로 조정 
		WaitTime = (OwnerEnemy->GetBehaviorType() == EEnemyBehaviorType::MovementTest) ? 5.f : FMath::Lerp(5.f, 30.f, RandBias); // 5 ~ 30 사이로 매핑
		// WaitTime = FMath::RandRange(5.f, 30.f);
	}
	
	Blackboard->SetValueAsEnum(IdleTaskKey, static_cast<uint8>(Type));
	return false;
}

EIdleTaskType UC_BehaviorComponent::GetIdleTaskType() const
{
	return static_cast<EIdleTaskType>(Blackboard->GetValueAsEnum(IdleTaskKey));
}

void UC_BehaviorComponent::OnTargetCharacterDead(AC_BasicCharacter* DeadCharacter)
{
	if (GetTargetCharacter() != DeadCharacter)
	{
		UC_Util::Print("From UC_BehaviorComponent::OnTargetCharacterDead : "
				 "Received Dead delegate callback, but TargetCharacter mismatched with dead character!", FColor::Red, 10.f);
		return;
	}
	SetTargetCharacter(nullptr);
	OwnerEnemyAIController->UpdateDetectedCharactersRangeLevel();
	OwnerEnemyAIController->TrySetTargetCharacterBasedOnPriority();
}

bool UC_BehaviorComponent::SetTargetCharacter(AC_BasicCharacter* InTargetCharacter)
{
	if (!IsValid(InTargetCharacter))
	{
		// Valid하지 않은 TargetCharacter의 경우, TargetCharacter를 놓쳤다고 판단(TargetCharacter의 사망 등)

		// 이전 TargetCharacter가 존재한다면, OnDead delegate 구독 끊기
		AC_BasicCharacter* TargetCharacter = GetTargetCharacter();
		if (IsValid(TargetCharacter)) TargetCharacter->Delegate_OnCharacterDead.RemoveAll(this);
		
		Blackboard->SetValueAsObject(TargetCharacterKey, nullptr);
		OwnerEnemy->SetTargetCharacterWidgetName("NONE"); // TODO : 이 라인 지우기 (For Testing)
		return false;
	}

	// TargetCharacter가 이미 죽었을 때
	if (InTargetCharacter->GetMainState() == EMainState::DEAD) return false; 

	// 현재 공격중인 때에는 TargetCharacter를 새로 바꾸지 않음
	if (GetServiceType() == EServiceType::COMBAT) return false;
	
	// 새로운 TargetCharacter로 세팅 이전, 이전 TargetCharacter OnDead delegate 구독 끊기
	if (AC_BasicCharacter* PrevTargetCharacter = GetTargetCharacter())
		PrevTargetCharacter->Delegate_OnCharacterDead.RemoveAll(this);

	// 새로운 TargetCharacter로 새로 setting 처리
	
	Blackboard->SetValueAsObject(TargetCharacterKey, InTargetCharacter);

	InTargetCharacter->Delegate_OnCharacterDead.AddUObject(this, &UC_BehaviorComponent::OnTargetCharacterDead);
	
	OwnerEnemy->SetTargetCharacterWidgetName(InTargetCharacter->GetCharacterName()); // TODO : 이 라인 지우기(For Testing)
	
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

AC_BasicCharacter* UC_BehaviorComponent::GetTargetCharacter() const
{
	return Cast<AC_BasicCharacter>(Blackboard->GetValueAsObject(TargetCharacterKey));
}


