// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/CombatTask/C_BTTaskAttack.h"

#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Item/Weapon/C_Weapon.h"

#include "AI/C_EnemyAIController.h"
#include "AI/C_BehaviorComponent.h"
#include "Utility/C_Util.h"

#include "Singleton/C_GameSceneManager.h"


UC_BTTaskAttack::UC_BTTaskAttack()
{
	bNotifyTick = true;
}

void UC_BTTaskAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

EBTNodeResult::Type UC_BTTaskAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (!IsValid(EnemyEquippedComponent))
	{
		AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetOwner());
		if (!IsValid(Controller))
		{
			UC_Util::Print("From BTTaskAttack ExecuteTask : Controller Casting failed!", FColor::Red, 10.f);
			return EBTNodeResult::Failed;
		}

		AC_Enemy* OwnerEnemy = Cast<AC_Enemy>(Controller->GetPawn());
		if (!IsValid(OwnerEnemy))
		{
			UC_Util::Print("From BTTaskAttack ExecuteTask : Enemy Casting failed!", FColor::Red, 10.f);
			return EBTNodeResult::Failed;
		}

		EnemyEquippedComponent = OwnerEnemy->GetEquippedComponent();
	}

	// TODO : Focused target이 필요함 (우선은 플레이어로 처리)
	AC_Player* PlayerTarget = GAMESCENE_MANAGER->GetPlayer();
	if (!EnemyEquippedComponent->GetCurWeapon()) return EBTNodeResult::Succeeded;

	bool AttackSucceeded = EnemyEquippedComponent->GetCurWeapon()->ExecuteAIAttack(PlayerTarget);

	// TODO : TickTask에서 공격을 계속해서 이어나갈지 고민 중
	return AttackSucceeded ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}


