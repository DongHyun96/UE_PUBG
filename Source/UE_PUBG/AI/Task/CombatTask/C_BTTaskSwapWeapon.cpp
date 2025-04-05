// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/CombatTask/C_BTTaskSwapWeapon.h"

#include "Character/Component/C_EquippedComponent.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Enemy.h"

#include "AI/C_EnemyAIController.h"
#include "AI/C_BehaviorComponent.h"
#include "AI/Service/C_BTServiceCombat.h"
#include "Character/Component/C_InvenComponent.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"

#include "Utility/C_Util.h"

UC_BTTaskSwapWeapon::UC_BTTaskSwapWeapon()
{
	bNotifyTick = true;
}

void UC_BTTaskSwapWeapon::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetOwner());
	if (!IsValid(Controller))
	{
		UC_Util::Print("From BTTaskSwapWeapon ExecuteTask : Controller Casting failed!", FColor::Red, 10.f);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AC_Enemy* Enemy = Cast<AC_Enemy>(Controller->GetPawn());
	UC_BehaviorComponent* BehaviorComponent = Controller->GetBehaviorComponent();
	
	// TODO : 현재는 Testing용 처리 중
	UC_EquippedComponent* EquippedComponent = Enemy->GetEquippedComponent();
	if (EquippedComponent->GetCurWeaponType() != SwapTargetWeaponSlotMap[Enemy]) return;

	// 여기서부터 초 세기
	TotalTimeMap[Enemy] += DeltaSeconds;

	if (TotalTimeMap[Enemy] > 2.f) // Task 끝났다고 간주
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		TotalTimeMap.Remove(Enemy);
		SwapTargetWeaponSlotMap.Remove(Enemy);
	}
}

EBTNodeResult::Type UC_BTTaskSwapWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetOwner());
	if (!IsValid(Controller))
	{
		UC_Util::Print("From BTTaskSwapWeapon ExecuteTask : Controller Casting failed!", FColor::Red, 10.f);
		return EBTNodeResult::Failed;
	}

	AC_Enemy*			  Enemy				= Cast<AC_Enemy>(Controller->GetPawn());
	UC_BehaviorComponent* BehaviorComponent = Controller->GetBehaviorComponent();
	AC_BasicCharacter*	  TargetCharacter	= BehaviorComponent->GetTargetCharacter();
	
	if (!IsValid(TargetCharacter)) return EBTNodeResult::Failed;

	float DistanceToTargetCharacter = FVector::Distance(Enemy->GetActorLocation(), TargetCharacter->GetActorLocation());

	// 거리가 50m를 넘어가면 SR들기
	if (DistanceToTargetCharacter > 1000.f)
		return ExecuteWeaponSwapRoutine(EWeaponSlot::SUB_GUN, Enemy, BehaviorComponent);

	// 거리 50m 이내 / 현재 시야에 보이는 중이라면 MainGun으로 교체 / 시야에 보이지 않는 중이라면 ThrowableWeapon 중 택 1
	if (Controller->IsCurrentlyOnSight(TargetCharacter))
		return ExecuteWeaponSwapRoutine(EWeaponSlot::MAIN_GUN, Enemy, BehaviorComponent);

	// 시야에 보이지 않는 중, Inven에 있는 Throwable이랑 Slot에 있는 Throwable 모두 조사해야 함

	// Init Throwable Swap Descriptor
	FThrowableSwapExecutionDescriptor Descriptor{};
	bool bHasGrenade{}, bHasFlashBang{}; // Slot이나 Inven에 해당 무기가 있는지 체크
	
	Descriptor.Enemy				= Enemy;
	Descriptor.BehaviorComponent	= BehaviorComponent;

	AC_ThrowingWeapon* CurrentSlotThrowableWeapon = Cast<AC_ThrowingWeapon>(Enemy->GetEquippedComponent()->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON]);
	Descriptor.CurrentThrowableSlotType = EThrowableType::MAX;

	if (CurrentSlotThrowableWeapon) // 투척류 슬롯에 장착된 투척류가 존재한다면 해당 투척류 종류 조사
	{
		Descriptor.CurrentThrowableSlotType = CurrentSlotThrowableWeapon->GetThrowableType();
		bHasGrenade							= Descriptor.CurrentThrowableSlotType == EThrowableType::GRENADE;
		bHasFlashBang						= Descriptor.CurrentThrowableSlotType == EThrowableType::FLASH_BANG;
	}

	FName GrenadeItemName		= AC_ThrowingWeapon::GetThrowableItemName(EThrowableType::GRENADE);
	FName FlashBangItemName 	= AC_ThrowingWeapon::GetThrowableItemName(EThrowableType::FLASH_BANG);
	Descriptor.InvenGrenade 	= Cast<AC_ThrowingWeapon>(Enemy->GetInvenComponent()->FindMyItemByName(GrenadeItemName));
	Descriptor.InvenFlashBang	= Cast<AC_ThrowingWeapon>(Enemy->GetInvenComponent()->FindMyItemByName(FlashBangItemName));
	
	bHasGrenade   = bHasGrenade   || Descriptor.InvenGrenade;
	bHasFlashBang = bHasFlashBang || Descriptor.InvenFlashBang;

	// Descriptor.CurrentWeaponType = Enemy->GetEquippedComponent()->GetCurWeaponType();
	
	if (bHasGrenade && bHasFlashBang) // Grenade와 FlashBang이 둘 다 존재할 때
	{
		// Grenade 70% 확률 | FlashBang 30% 확률
		if (FMath::RandRange(0.f, 1.f) < 0.7f) // Grenade 장착
		{
			Descriptor.InChangeTo = EThrowableType::GRENADE;
			EBTNodeResult::Type ExecuteSwapResult = ExecuteThrowableSwapRoutine(Descriptor);
			
			if (ExecuteSwapResult != EBTNodeResult::Failed) // Grenade 장착 시도 실패했다면 FlashBang도 장착 시도도 가능하게끔 남겨둠
				return ExecuteSwapResult;
		}
		
		// 30%의 확률로 FlashBang 장착
		Descriptor.InChangeTo = EThrowableType::FLASH_BANG;
		return ExecuteThrowableSwapRoutine(Descriptor);
	}

	if (bHasGrenade)
	{
		Descriptor.InChangeTo = EThrowableType::GRENADE;
		return ExecuteThrowableSwapRoutine(Descriptor);
	}

	if (bHasFlashBang)
	{
		Descriptor.InChangeTo = EThrowableType::FLASH_BANG;
		return ExecuteThrowableSwapRoutine(Descriptor);
	}

	// 아무 공격 투척류가 없을 때
	return EBTNodeResult::Failed;
}


EBTNodeResult::Type UC_BTTaskSwapWeapon::ExecuteWeaponSwapRoutine(EWeaponSlot InChangeTo, AC_Enemy* Enemy, UC_BehaviorComponent* BehaviorComponent)
{
	// 이미 해당 무기를 손에 들고 있을 때에는 Succeeded 처리하고 종료
	if (Enemy->GetEquippedComponent()->GetCurWeaponType() == InChangeTo)
	{
		BehaviorComponent->SetCombatTaskType(ECombatTaskType::ATTACK);
		return EBTNodeResult::Succeeded;
	}

	if (!Enemy->GetEquippedComponent()->ChangeCurWeapon(InChangeTo)) return EBTNodeResult::Failed;
	
	SwapTargetWeaponSlotMap.Add(Enemy, InChangeTo);
	TotalTimeMap.Add(Enemy, 0.f);
	
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UC_BTTaskSwapWeapon::ExecuteThrowableSwapRoutine(const FThrowableSwapExecutionDescriptor& Desc)
{
	if (Desc.CurrentThrowableSlotType == Desc.InChangeTo) // Slot에 이미 해당무기가 장착되어있는 상황
		return ExecuteWeaponSwapRoutine(EWeaponSlot::THROWABLE_WEAPON, Desc.Enemy, Desc.BehaviorComponent);

	// Inven에 있는 해당 투척류 Slot으로 옮기고 무기 바꾸기
	AC_ThrowingWeapon* InvenThrowableWeapon = (Desc.InChangeTo == EThrowableType::GRENADE) ? Desc.InvenGrenade : Desc.InvenFlashBang;

	// Inven의 Throwable MoveToSlot 실패 시, Failed 처리
	if (!InvenThrowableWeapon->MoveToSlot(Desc.Enemy, InvenThrowableWeapon->GetItemCurStack()))	return EBTNodeResult::Failed;

	EWeaponSlot CurrentWeaponType = Desc.Enemy->GetEquippedComponent()->GetCurWeaponType();

	// 현재 Throwable 무기를 들고 있지 않을 때에 ChangeCurWeapon 시도
	if (CurrentWeaponType != EWeaponSlot::THROWABLE_WEAPON)
	{
		if (!Desc.Enemy->GetEquippedComponent()->ChangeCurWeapon(EWeaponSlot::THROWABLE_WEAPON)) 
			return EBTNodeResult::Failed;
	}

	// 다른 Throwable을 들고 있었을 경우, 위에서 MoveToSlot함과 동시에 들고 있는 Throwable 종류가 해당 무기로 바뀜
	SwapTargetWeaponSlotMap.Add(Desc.Enemy, EWeaponSlot::THROWABLE_WEAPON);
	TotalTimeMap.Add(Desc.Enemy, 0.f);
	return EBTNodeResult::InProgress;
}
