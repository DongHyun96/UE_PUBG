// Fill out your copyright notice in the Description page of Project Settings.


#include "C_BTTaskSmokeGrenade.h"

#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"
#include "AI/Task/CombatTask/C_BTTaskSwapWeapon.h"
#include "Character/C_Enemy.h"
#include "Character/Component/C_InvenComponent.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Utility/C_Util.h"

UC_BTTaskSmokeGrenade::UC_BTTaskSmokeGrenade()
{
	bNotifyTick = true;
}

void UC_BTTaskSmokeGrenade::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetAIOwner());
	if (!IsValid(Controller))
	{
		UC_Util::Print("From UC_BTTaskSmokeGrenade::TickTask : Controller casting failed!", FColor::Red, 10.f);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 현재 FlashBang 피격 중인 상황(현상 유지)
	if (Controller->IsFlashBangEffectTimeLeft()) return;
	
	AC_Enemy* Enemy = Cast<AC_Enemy>(Controller->GetPawn());
	UC_BehaviorComponent* BehaviorComponent = Controller->GetBehaviorComponent();

	// 아직 Smoke Grenade ChangeWeapon 동작 처리 중
	if (Enemy->GetEquippedComponent()->GetIsCurrentlyChangingWeapon()) return;

	// ChangeWeapon 동작 처리 끝, 여기서 남은 Montage 시간이 있을 수 있어 0.5초 기다림
	if (!EnemyTickTaskTimer.Contains(Enemy))
	{
		// 첫 시작
		EnemyTickTaskTimer.Add(Enemy, 0.f);
		return;
	}

	EnemyTickTaskTimer[Enemy] += DeltaSeconds;
	if (EnemyTickTaskTimer[Enemy] < 0.5f) return;

	UC_EquippedComponent* EquippedComponent = Enemy->GetEquippedComponent();

	if (EquippedComponent->GetCurWeaponType() != EWeaponSlot::THROWABLE_WEAPON)
	{
		UC_Util::Print("From UC_BTTaskSmokeGrenade::TickTask : Changed Weapon type is not throwable!, continue next sequence anyway...", FColor::Red, 10.f);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		EnemyTickTaskTimer.Remove(Enemy);
		return;
	}

	AC_Weapon* CurWeapon = EquippedComponent->GetCurWeapon();
	AC_BasicCharacter* TargetCharacter = BehaviorComponent->GetTargetCharacter();

	if (Cast<AC_ThrowingWeapon>(CurWeapon)->GetThrowableType() != EThrowableType::SMOKE)
	{
		UC_Util::Print("From UC_BTTaskSmokeGrenade::TickTask : Cur Weapon Throwable Type not Smoke Grenade!, continue next sequence anyway...", FColor::Red, 10.f);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		EnemyTickTaskTimer.Remove(Enemy);
		return;
	}

	// ExecuteAIAttack이 아직 호출 처리 안된 케이스
	if (!SmokeGrenadeAIAttackExecutedEnemies.Contains(Enemy))
	{
		bool ExecuteAttackStrategySucceeded = CurWeapon->ExecuteAIAttack(TargetCharacter);
		if (!ExecuteAttackStrategySucceeded)
		{
			UC_Util::Print("From UC_BTTaskSmokeGrenade::TickTask : Execute AI Attack failed!, continue next sequence anyway...", FColor::Red, 10.f);
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			EnemyTickTaskTimer.Remove(Enemy);
			return;
		}
		SmokeGrenadeAIAttackExecutedEnemies.Add(Enemy);
		return;
	}

	// ExecuteAIAttack 호출 이후, Tick 부분
	if (!CurWeapon->ExecuteAIAttackTickTask(TargetCharacter, DeltaSeconds))
	{
		SmokeGrenadeAIAttackExecutedEnemies.Remove(Enemy);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		EnemyTickTaskTimer.Remove(Enemy);
	}
}

EBTNodeResult::Type UC_BTTaskSmokeGrenade::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetAIOwner());

	if (!IsValid(Controller))
	{
		UC_Util::Print("From UC_BTTaskSmokeGrenade::ExecuteTask : Controller casting failed!", FColor::Red, 10.f);
		return EBTNodeResult::Failed;
	}
	
	AC_Enemy* Enemy = Cast<AC_Enemy>(Controller->GetPawn());
	UC_EquippedComponent* EquippedComponent = Enemy->GetEquippedComponent();

	// Smoke Grenade 있다면 바꿔들고, 사용하기
	AC_ThrowingWeapon* CurrentSlotThrowableWeapon = Cast<AC_ThrowingWeapon>(EquippedComponent->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON]);
	bool bHasSmokeGrenade{};
	EThrowableType CurrentSlotThrowableType = EThrowableType::MAX;
	if (CurrentSlotThrowableWeapon)
	{
		CurrentSlotThrowableType = CurrentSlotThrowableWeapon->GetThrowableType();
		bHasSmokeGrenade = CurrentSlotThrowableType == EThrowableType::SMOKE;
	}
	
	FName SmokeGrenadeItemName = AC_ThrowingWeapon::GetThrowableItemName(EThrowableType::SMOKE);
	AC_ThrowingWeapon* InvenSmokeGrenade = Cast<AC_ThrowingWeapon>(Enemy->GetInvenComponent()->FindMyItemByName(SmokeGrenadeItemName));
	
	bHasSmokeGrenade = bHasSmokeGrenade || InvenSmokeGrenade;
	if (!bHasSmokeGrenade) return EBTNodeResult::Succeeded; // Smoke grenade를 가지고 있지 않은 상황 -> 다음 Task sequence로 진행

	// 기본적으로 40%의 확률로 SmokeGrenade 사용 처리, but 만약 TargetCharacter가 시야에 들어오는 상황이라면 무조건 사용 시도
	bool bIsTargetCharacterOnSight = Controller->IsCurrentlyOnSight(Controller->GetBehaviorComponent()->GetTargetCharacter()); 
	if (!bIsTargetCharacterOnSight && FMath::RandRange(0.f, 1.f) < 0.6f) return EBTNodeResult::Succeeded;

	if (CurrentSlotThrowableType == EThrowableType::SMOKE) // Slot에 이미 SmokeGrenade가 장착되어 있는 상황
	{
		if (EquippedComponent->GetCurWeaponType() == EWeaponSlot::THROWABLE_WEAPON) // 이미 SmokeGrenade를 손에 들고 있는 상황
			return EBTNodeResult::InProgress;

		// Slot에 SmokeGrenade가 이미 장착되어 있고, 손에 들고 있지는 않은 상황
		bool ChangeCurWeaponToThrowable = EquippedComponent->ChangeCurWeapon(EWeaponSlot::THROWABLE_WEAPON); 
		return ChangeCurWeaponToThrowable ? EBTNodeResult::InProgress : EBTNodeResult::Succeeded; // ChangeCurWeapon 실패 시 다음 Sequence task 실행
	}

	// Slot에 Smoke Grenade가 장착되어있지 않은 상황, Inven에 있는 Smoke Grenade Slot으로 옮기고 무기 바꾸기
	if (!InvenSmokeGrenade->MoveToSlot(Enemy, InvenSmokeGrenade->GetItemCurStack()))
		return EBTNodeResult::Succeeded;// 제대로 옮겨지지 않았다면 다음 Task sequence로 넘어감

	// Slot에 새로 들어온 SmokeGrenade CurMontage 업데이트 시키기(MoveToSlot으로 새로 만들어진 투척류는 Tick이 한번도 호출되지 않아서 CurMontages가 제대로 초기화되어 있지 않음)
	AC_ThrowingWeapon* NewSlotSmokeGrenade = Cast<AC_ThrowingWeapon>(EquippedComponent->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON]);
	if (!NewSlotSmokeGrenade)
	{
		UC_Util::Print("From BTTask SmokeGrenade ExecuteTask : After MoveToSlot -> But Throwable Slot Weapon casting failed!", FColor::Red, 10.f);
		return EBTNodeResult::Succeeded; // 다음 Sequence task 실행
	}
	if (!NewSlotSmokeGrenade->UpdateCurMontagesToOwnerCharacterPoseState())
	{
		UC_Util::Print("From BTTask SmokeGrenade ExecuteTask : New SlotWeapon update current Montages failed!", FColor::Red, 10.f);
		return EBTNodeResult::Succeeded; // 다음 Sequence task 실행
	}

	// 현재 Throwable 무기를 들고 있지 않을 때에 ChangeCurWeapon 시도
	if (EquippedComponent->GetCurWeaponType() != EWeaponSlot::THROWABLE_WEAPON)
	{
		if (!EquippedComponent->ChangeCurWeapon(EWeaponSlot::THROWABLE_WEAPON))
		{
			UC_Util::Print("From BTTask SmokeGrenade ExecuteTask : ChangeCurWeapon failed!", FColor::Red, 10.f);
			return EBTNodeResult::Succeeded; // 다음 Sequence task 실행
		}
	}

	// 다른 Throwable을 들고 있었을 경우, 위에서 MoveToSlot함과 동시에 들고 있는 Throwable 종류가 SMOKE로 바뀜
	return EBTNodeResult::InProgress;
}
