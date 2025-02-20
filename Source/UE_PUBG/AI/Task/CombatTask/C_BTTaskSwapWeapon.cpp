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

	// TODO : 현재는 Testing용 처리 중
	UC_EquippedComponent* EquippedComponent = OwnerEnemy->GetEquippedComponent();
	if (EquippedComponent->GetCurWeaponType() != SwapTargetWeaponSlot) return;

	// 여기서부터 초 세기
	TotalTime += DeltaSeconds;

	if (TotalTime > 1.5f) // Task 끝났다고 간주
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		TotalTime = 0.f;
	}
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
	// TODO : SwapWeapon Type 지정해서 해당 타입으로 지정하기
	SwapTargetWeaponSlot = EWeaponSlot::MAIN_GUN;
	bool Succeeded = OwnerEnemy->GetEquippedComponent()->ChangeCurWeapon(SwapTargetWeaponSlot);

	/*Succeeded,
	Failed,
	Aborted,
	InProgress,*/
	
	// 무기를 바꿀 수 있는 상황이라면 무기를 바꾸기 까지 animation 동작 기다리기
	return Succeeded ? EBTNodeResult::InProgress : EBTNodeResult::Failed;
}


