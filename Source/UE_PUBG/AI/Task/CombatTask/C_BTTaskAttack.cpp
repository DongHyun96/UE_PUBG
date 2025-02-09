// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/CombatTask/C_BTTaskAttack.h"

#include "EdGraphSchema_K2_Actions.h"
#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
#include "Character/Component/C_EquippedComponent.h"
#include "AI/C_BehaviorComponent.h"
#include "Item/Weapon/C_Weapon.h"

#include "AI/C_EnemyAIController.h"
#include "Utility/C_Util.h"


#include "Singleton/C_GameSceneManager.h"

UC_BTTaskAttack::UC_BTTaskAttack()
{
	bNotifyTick = true;
}

void UC_BTTaskAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// TODO : 지속적으로 몇 초간 현재 무기로 공격한다고 하면 TickTask 사용
}

EBTNodeResult::Type UC_BTTaskAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// First initialization
	if (!IsValid(OwnerEnemy))
	{
		AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetOwner());
		if (!IsValid(Controller))
		{
			UC_Util::Print("From BTTaskAttack::ExecuteTask : Controller Casting failed!", FColor::Red, 10.f);
			return EBTNodeResult::Failed;
		}

		OwnerEnemy = Cast<AC_Enemy>(Controller->GetPawn());
		if (!OwnerEnemy)
		{
			UC_Util::Print("From BTTaskAttack::ExecuteTask : Enemy Actor Casting failed!", FColor::Red, 10.f);
			return EBTNodeResult::Failed;
		}

		OwnerBehaviorComponent = Controller->GetBehaviorComponent();
	}

	UC_EquippedComponent* EnemyEquippedComponent = OwnerEnemy->GetEquippedComponent(); 

	// 무기를 들고 있지 않은 상황
	if (!EnemyEquippedComponent->GetCurWeapon() || OwnerEnemy->GetHandState() == EHandState::UNARMED)
	{
		UC_Util::Print("From UC_BTTaskAttack::ExecuteTask : Not holding any weapons", FColor::Red, 10.f);
		return EBTNodeResult::Failed;
	}

	// TargetCharacter가 없는 상황
	AC_BasicCharacter* TargetCharacter = Cast<AC_BasicCharacter>(OwnerBehaviorComponent->GetTargetCharacter());
	if (!IsValid(TargetCharacter))
	{
		UC_Util::Print("From UC_BTTaskAttack::ExecuteTask : Invalid TargetCharacter", FColor::Red, 10.f);
		return EBTNodeResult::Failed;
	}

	bool AttackSucceeded = EnemyEquippedComponent->GetCurWeapon()->ExecuteAIAttack(TargetCharacter);

	if (AttackSucceeded)	UC_Util::Print("Attack Succeeded", FColor::Red, 10.f);	
	else					UC_Util::Print("Attack Failed", FColor::Red, 10.f);

	OwnerBehaviorComponent->SetServiceType(EServiceType::IDLE);

	// TODO : TickTask에서 공격을 계속해서 이어나갈지 고민 중 -> 일단 단일 공격 한 번으로 Task 끝내는 것으로 처리함
	return AttackSucceeded ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}


