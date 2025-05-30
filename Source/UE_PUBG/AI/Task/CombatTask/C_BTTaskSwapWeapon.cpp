// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/CombatTask/C_BTTaskSwapWeapon.h"

#include "Character/Component/C_EquippedComponent.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Enemy.h"

#include "AI/C_EnemyAIController.h"
#include "AI/C_BehaviorComponent.h"
#include "AI/Service/C_BTServiceCombat.h"
#include "Character/Component/C_InvenComponent.h"
#include "Item/ItemBullet/C_Item_Bullet.h"
#include "Item/Weapon/Gun/C_Gun.h"

#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Singleton/C_GameSceneManager.h"

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

	// 현재 FlashBang 피격 중인 상황(현상 유지)
	if (Controller->IsFlashBangEffectTimeLeft()) return;

	AC_Enemy* Enemy = Cast<AC_Enemy>(Controller->GetPawn());
	UC_BehaviorComponent* BehaviorComponent = Controller->GetBehaviorComponent();
	UC_EquippedComponent* EquippedComponent = Enemy->GetEquippedComponent();

	/*switch (EquippedComponent->GetCurWeaponType())
	{
	case EWeaponSlot::NONE: UC_Util::Print("NONE"); break;
	case EWeaponSlot::MAIN_GUN: UC_Util::Print("MAIN_GUN"); break;
	case EWeaponSlot::SUB_GUN: UC_Util::Print("SUB_GUN"); break;
	case EWeaponSlot::MELEE_WEAPON: UC_Util::Print("MELEE_WEAPON"); break;
	case EWeaponSlot::THROWABLE_WEAPON: UC_Util::Print("THROWABLE_WEAPON"); break;
	}*/
	
	if (EquippedComponent->GetCurWeaponType() != SwapTargetWeaponSlotMap[Enemy]) return;
	if (EquippedComponent->GetIsCurrentlyChangingWeapon()) return;

	// 여기서부터 초 세기
	TotalTimeMap[Enemy] += DeltaSeconds;
	
	if (TotalTimeMap[Enemy] > 0.5f) // Task 끝났다고 간주
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
	
	if (!IsValid(TargetCharacter)) return ExecuteTaskReturnRoutine(EBTNodeResult::Failed, BehaviorComponent);

	// 도중에 상태가 바뀌었다면 Failed 처리하고 돌아가기
	if (BehaviorComponent->GetServiceType() != EServiceType::COMBAT) return EBTNodeResult::Failed;

	// TODO : /**/ --> 이 부분 주석 풀기
	
	// 총기류 먼저 바꾸기 시도
	FName SevenBulletItemName	= AC_Item_Bullet::GetBulletTypeName(EBulletType::SEVENMM);
	FName FiveBulletItemName	= AC_Item_Bullet::GetBulletTypeName(EBulletType::FIVEMM);
	
	int TotalSevenBulletCount	= Enemy->GetInvenComponent()->GetTotalStackByItemName(SevenBulletItemName);
	int TotalFiveBulletCount	= Enemy->GetInvenComponent()->GetTotalStackByItemName(FiveBulletItemName);

	AC_Gun* MainGun = Cast<AC_Gun>(Enemy->GetEquippedComponent()->GetWeapons()[EWeaponSlot::MAIN_GUN]);
	TotalFiveBulletCount += MainGun->GetCurBulletCount();

	AC_Gun* SubGun = Cast<AC_Gun>(Enemy->GetEquippedComponent()->GetWeapons()[EWeaponSlot::SUB_GUN]);
	TotalSevenBulletCount += SubGun->GetCurBulletCount();

	float DistanceToTargetCharacter = FVector::Distance(Enemy->GetActorLocation(), TargetCharacter->GetActorLocation());
	
	// 남은 5탄과 7탄이 모두 있을 때
	if (TotalSevenBulletCount > 0 && TotalFiveBulletCount > 0)
	{

		// 거리가 50m를 넘어가면 SubGun으로 교체 시도
		if (DistanceToTargetCharacter > 500.f) 
			return ExecuteWeaponSwapRoutine(EWeaponSlot::SUB_GUN, Enemy, BehaviorComponent); 

		// 거리 50m 이내 / 현재 시야에 보이는 중이라면 MainGun으로 교체 / 시야에 보이지 않는 중이라면 ThrowableWeapon 중 택 1 (역시 남은 장탄수 확인해서)
		if (Controller->IsCurrentlyOnSight(TargetCharacter))
			return ExecuteWeaponSwapRoutine(EWeaponSlot::MAIN_GUN, Enemy, BehaviorComponent);
	}
	else if (TotalSevenBulletCount > 0) // 7탄만 남았을 때
	{
		// 거리가 50m를 넘어가면 SubGun으로 교체 시도
		if (DistanceToTargetCharacter > 500.f) // TODO : 50m로 수정
			return ExecuteWeaponSwapRoutine(EWeaponSlot::SUB_GUN, Enemy, BehaviorComponent);

		// 50m 이내일 때, 시야에 보이면 SubGun 교체 시도
		if (Controller->IsCurrentlyOnSight(TargetCharacter))
			return ExecuteWeaponSwapRoutine(EWeaponSlot::SUB_GUN, Enemy, BehaviorComponent);
	}
	else if (TotalFiveBulletCount > 0) // 5탄만 남았을 때
	{
		// 시야에 보이는지만 체크해서 AR 사용하기
		if (Controller->IsCurrentlyOnSight(TargetCharacter))
			return ExecuteWeaponSwapRoutine(EWeaponSlot::MAIN_GUN, Enemy, BehaviorComponent);
	}

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
			UC_Util::Print("70% Execute Swap to Grenade", GAMESCENE_MANAGER->GetTickRandomColor(), 10.f);
			Descriptor.InChangeTo = EThrowableType::GRENADE;
			EBTNodeResult::Type ExecuteSwapResult = ExecuteThrowableSwapRoutine(Descriptor);

			// Grenade 장착 시도 실패했다면 FlashBang도 장착 시도도 가능하게끔 남겨둠
			if (ExecuteSwapResult != EBTNodeResult::Failed) return ExecuteSwapResult;
		}
		
		// 30%의 확률로 FlashBang 장착
		UC_Util::Print("30% Execute Swap to FlashBang", GAMESCENE_MANAGER->GetTickRandomColor(), 10.f);
		Descriptor.InChangeTo = EThrowableType::FLASH_BANG;
		return ExecuteThrowableSwapRoutine(Descriptor);
	}

	if (bHasGrenade)
	{
		UC_Util::Print("Execute Swap to Grenade", GAMESCENE_MANAGER->GetTickRandomColor(), 10.f);
		Descriptor.InChangeTo = EThrowableType::GRENADE;
		return ExecuteThrowableSwapRoutine(Descriptor);
	}

	if (bHasFlashBang)
	{
		UC_Util::Print("Execute Swap to FlashBang", GAMESCENE_MANAGER->GetTickRandomColor(), 10.f);
		Descriptor.InChangeTo = EThrowableType::FLASH_BANG;
		return ExecuteThrowableSwapRoutine(Descriptor);
	}

	// 아무 공격 투척류 종류가 없을 때
	// UC_Util::Print("Tried Swap to throwable, but no attacking throwable available!", GAMESCENE_MANAGER->GetTickRandomColor(), 10.f);
	return ExecuteTaskReturnRoutine(EBTNodeResult::Failed, BehaviorComponent);
}


EBTNodeResult::Type UC_BTTaskSwapWeapon::ExecuteWeaponSwapRoutine(EWeaponSlot InChangeTo, AC_Enemy* Enemy, UC_BehaviorComponent* BehaviorComponent)
{
	/*
	 * 1. 이미 해당 무기를 손에 들고 있을 때 Succeeded 처리
	 * 2. ChangeCurWeapon 시도 Failed -> Failed 처리
	 * 3. 무기를 바꾸는 Anim Montage 재생 -> InProgress 처리
	 */
	return	Enemy->GetEquippedComponent()->GetCurWeaponType() == InChangeTo	? 	ExecuteTaskReturnRoutine(EBTNodeResult::Succeeded, BehaviorComponent) :
			!Enemy->GetEquippedComponent()->ChangeCurWeapon(InChangeTo)		? 	ExecuteTaskReturnRoutine(EBTNodeResult::Failed, BehaviorComponent) :
																				ExecuteTaskReturnRoutine(EBTNodeResult::InProgress, BehaviorComponent, Enemy, InChangeTo);  
}

EBTNodeResult::Type UC_BTTaskSwapWeapon::ExecuteThrowableSwapRoutine(const FThrowableSwapExecutionDescriptor& Desc)
{
	if (Desc.CurrentThrowableSlotType == Desc.InChangeTo) // Slot에 이미 해당무기가 장착되어있는 상황
	{
		UC_Util::Print("Target Throwable Type already on slot!", GAMESCENE_MANAGER->GetTickRandomColor(), 10.f);
		return ExecuteWeaponSwapRoutine(EWeaponSlot::THROWABLE_WEAPON, Desc.Enemy, Desc.BehaviorComponent);
	}

	// Inven에 있는 해당 투척류 Slot으로 옮기고 무기 바꾸기
	UC_Util::Print("Try Move Inven Throwable to Throwable Slot", GAMESCENE_MANAGER->GetTickRandomColor(), 10.f);
	AC_ThrowingWeapon* InvenThrowableWeapon = (Desc.InChangeTo == EThrowableType::GRENADE) ? Desc.InvenGrenade : Desc.InvenFlashBang;

	// Inven의 Throwable MoveToSlot 실패 시, Failed 처리
	if (!InvenThrowableWeapon->MoveToSlot(Desc.Enemy, InvenThrowableWeapon->GetItemCurStack()))
	{
		UC_Util::Print("MoveToSlot Failed!", GAMESCENE_MANAGER->GetTickRandomColor(), 10.f);
		return ExecuteTaskReturnRoutine(EBTNodeResult::Failed, Desc.BehaviorComponent);
	}

	EWeaponSlot CurrentWeaponType = Desc.Enemy->GetEquippedComponent()->GetCurWeaponType();

	// Slot에 새로 들어온 무기 CurMontage 업데이트 시키기(MoveToSlot으로 새로 만들어진 투척류는 Tick이 한번도 호출되지 않아서 CurMontages가 제대로 초기화되어 있지 않음)
	AC_ThrowingWeapon* SlotThrowableWeapon =  Cast<AC_ThrowingWeapon>(Desc.Enemy->GetEquippedComponent()->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON]);
	if (!SlotThrowableWeapon)
	{
		UC_Util::Print("After Throwable MoveToSlot -> But Throwable Slot Weapon casting failed!", FColor::Red, 10.f);
		return ExecuteTaskReturnRoutine(EBTNodeResult::Failed, Desc.BehaviorComponent);
	}

	if (!SlotThrowableWeapon->UpdateCurMontagesToOwnerCharacterPoseState())
	{
		UC_Util::Print("New SlotWeapon update current Montages failed!", FColor::Red, 10.f);
		return ExecuteTaskReturnRoutine(EBTNodeResult::Failed, Desc.BehaviorComponent);
	}

	// 현재 Throwable 무기를 들고 있지 않을 때에 ChangeCurWeapon 시도
	if (CurrentWeaponType != EWeaponSlot::THROWABLE_WEAPON)
	{
		if (!Desc.Enemy->GetEquippedComponent()->ChangeCurWeapon(EWeaponSlot::THROWABLE_WEAPON)) 
			return ExecuteTaskReturnRoutine(EBTNodeResult::Failed, Desc.BehaviorComponent);
		
		UC_Util::Print("Change CurWeapon to Throwable succeeded!", GAMESCENE_MANAGER->GetTickRandomColor(), 10.f);
	}

	// 다른 Throwable을 들고 있었을 경우, 위에서 MoveToSlot함과 동시에 들고 있는 Throwable 종류가 해당 무기로 바뀜
	return ExecuteTaskReturnRoutine(EBTNodeResult::InProgress, Desc.BehaviorComponent, Desc.Enemy, EWeaponSlot::THROWABLE_WEAPON);
}

EBTNodeResult::Type UC_BTTaskSwapWeapon::ExecuteTaskReturnRoutine(const EBTNodeResult::Type& ReturnType, UC_BehaviorComponent* BehaviorComponent, AC_Enemy* Enemy, EWeaponSlot InChangeTo)
{
	switch (ReturnType)
	{
	case EBTNodeResult::Failed:
		BehaviorComponent->SetServiceType(EServiceType::IDLE);
		BehaviorComponent->SetIdleTaskType(EIdleTaskType::WAIT);
		break;
	case EBTNodeResult::InProgress:
		
		if (!Enemy || InChangeTo == EWeaponSlot::NONE)
		{
			UC_Util::Print
			(
				"From UC_BTTaskSwapWeapon::ExecuteTaskReturnRoutine : Received InProgress, but Enemy nullptr or invalid InChangeTo Type received!",
				FColor::Red, 10.f
			);
			break;
		}
		SwapTargetWeaponSlotMap.Add(Enemy, InChangeTo);
		TotalTimeMap.Add(Enemy, 0.f);
		break;
	case EBTNodeResult::Succeeded: case EBTNodeResult::Aborted: default: break;
	}
	return ReturnType;
}
