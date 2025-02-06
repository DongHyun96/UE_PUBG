// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/CombatTask/C_BTTaskSwapWeapon.h"

#include "Character/Component/C_EquippedComponent.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Enemy.h"

#include "AI/C_EnemyAIController.h"
#include "AI/C_BehaviorComponent.h"

#include "Utility/C_Util.h"

UC_BTTaskSwapWeapon::UC_BTTaskSwapWeapon()
{
	bNotifyTick = true;
}

void UC_BTTaskSwapWeapon::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	//static int SlotIterator = 1;
	//if (SlotIterator == 5) SlotIterator = 0;

	//TotalTime += DeltaSeconds;

	//if (TotalTime > 5.f) // Task 끝났다고 간주
	//{
	//	EWeaponSlot CurSlot = static_cast<EWeaponSlot>(SlotIterator++);

	//	OwnerEnemy->GetEquippedComponent()->ChangeCurWeapon(CurSlot);
	//	TotalTime = 0.f;
	//	//OwnerBehaviorComponent->SetServiceType(EServiceType::IDLE);
	//	//FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	//}
}

EBTNodeResult::Type UC_BTTaskSwapWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// Initialization
	if (!IsValid(OwnerEnemy))
	{
		AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetOwner());
		if (!IsValid(Controller))
		{
			UC_Util::Print("From BTTaskSwapWeapon ExecuteTask : Controller Casting failed!", FColor::Red, 10.f);
			return EBTNodeResult::Failed;
		}

		OwnerEnemy = Cast<AC_Enemy>(Controller->GetPawn());

		if (!IsValid(OwnerEnemy))
		{
			UC_Util::Print("From BTTaskSwapWeapon ExecuteTask : OwnerEnemy Casting failed!", FColor::Red, 10.f);
			return EBTNodeResult::Failed;
		}

		OwnerBehaviorComponent = Controller->GetBehaviorComponent();

		if (!IsValid(OwnerBehaviorComponent))
		{
			UC_Util::Print("From BTTaskSwapWeapon ExecuteTask : OwnerBehaviorComponent Casting failed!", FColor::Red, 10.f);
			return EBTNodeResult::Failed;
		}
	}
	
	// Testing
	bool Succeeded = OwnerEnemy->GetEquippedComponent()->ChangeCurWeapon(EWeaponSlot::SUB_GUN);

	/*Succeeded,
	Failed,
	Aborted,
	InProgress,*/
	
	// TickTask 없이 바로 종료
	return Succeeded ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}


