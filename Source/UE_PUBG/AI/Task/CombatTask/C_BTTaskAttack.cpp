// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/CombatTask/C_BTTaskAttack.h"

#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
#include "Character/Component/C_EquippedComponent.h"
#include "AI/C_BehaviorComponent.h"
#include "Item/Weapon/C_Weapon.h"

#include "AI/C_EnemyAIController.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"


UC_BTTaskAttack::UC_BTTaskAttack()
{
	bNotifyTick = true;
}

void UC_BTTaskAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AC_EnemyAIController* EnemyAIController = Cast<AC_EnemyAIController>(OwnerComp.GetAIOwner());
	if (!IsValid(EnemyAIController)) FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	
	AC_Enemy* Enemy = Cast<AC_Enemy>(EnemyAIController->GetPawn());
	if (!IsValid(Enemy)) FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

	// 현재 FlashBang 피격 중인 상황(현상 유지)
	if (EnemyAIController->IsFlashBangEffectTimeLeft()) return;

	if (!AttackingWeapons.Contains(Enemy))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	AC_Weapon* CurrentAttackingWeapon = AttackingWeapons[Enemy];
	if (!IsValid(CurrentAttackingWeapon))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		AttackingWeapons.Remove(Enemy);
		return;
	}

	// 지속적으로 몇 초간 현재 무기로 공격한다고 하면 TickTask 사용
	if (!CurrentAttackingWeapon->ExecuteAIAttackTickTask(EnemyAIController->GetBehaviorComponent()->GetTargetCharacter(), DeltaSeconds))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		AttackingWeapons.Remove(Enemy);
	}
}

EBTNodeResult::Type UC_BTTaskAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetOwner());
	if (!IsValid(Controller))
	{
		UC_Util::Print("From BTTaskAttack::ExecuteTask : Controller Casting failed!", FColor::Red, 10.f);
		return EBTNodeResult::Failed;
	}
	
	AC_Enemy* Enemy = Cast<AC_Enemy>(Controller->GetPawn());
	if (!Enemy)
	{
		UC_Util::Print("From BTTaskAttack::ExecuteTask : Enemy Actor Casting failed!", FColor::Red, 10.f);
		return EBTNodeResult::Failed;
	}
	
	UC_BehaviorComponent* BehaviorComponent = Controller->GetBehaviorComponent();
	UC_EquippedComponent* EquippedComponent = Enemy->GetEquippedComponent(); 

	// 무기를 들고 있지 않은 상황
	if (!EquippedComponent->GetCurWeapon() || Enemy->GetHandState() == EHandState::UNARMED)
	{
		if (GAMESCENE_MANAGER->GetEnemies().Num() <= 1)
			UC_Util::Print("From UC_BTTaskAttack::ExecuteTask : Not holding any weapons", FColor::Red, 10.f);
		
		return EBTNodeResult::Failed;
	}

	// TargetCharacter가 없는 상황
	AC_BasicCharacter* TargetCharacter = Cast<AC_BasicCharacter>(BehaviorComponent->GetTargetCharacter());
	if (!IsValid(TargetCharacter))
	{
		if (GAMESCENE_MANAGER->GetEnemies().Num() <= 1)
			UC_Util::Print("From UC_BTTaskAttack::ExecuteTask : Invalid TargetCharacter", FColor::Red, 10.f);
		
		
		return EBTNodeResult::Failed;
	}

	AC_Weapon* CurrentAttackingWeapon = EquippedComponent->GetCurWeapon();
	
	// TickTask가 필요한 무기의 경우, TickTask에서 Continue / 필요 없는 무기의 경우 TickTask에서 바로 Succeeded로 처리
	if (!CurrentAttackingWeapon->ExecuteAIAttack(TargetCharacter)) return EBTNodeResult::Failed;

	BehaviorComponent->SetAfterAttackTaskWaitTime(0.f);
	
	AttackingWeapons.Add(Enemy, CurrentAttackingWeapon);
	return EBTNodeResult::InProgress;
}


