// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/IdleTask/C_BTTaskStatCare.h"

#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"
#include "Character/C_Enemy.h"
#include "Character/Component/C_InvenComponent.h"
#include "Item/ConsumableItem/C_ConsumableItem.h"
//#include "Item/ConsumableItem/C_ConsumableItem.h
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"

UC_BTTaskStatCare::UC_BTTaskStatCare()
{
	bNotifyTick = true;
}

void UC_BTTaskStatCare::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetAIOwner());
	if (!IsValid(Controller))
	{
		UC_Util::Print("From UC_BTTaskStatCare TickTask : Controller Casting failed!", GAMESCENE_MANAGER->GetTickRandomColor(), 10.f);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 현재 FlashBang 피격 중인 상황(현상 유지)
	if (Controller->IsFlashBangEffectTimeLeft()) return;

	AC_Enemy* Enemy = Cast<AC_Enemy>(Controller->GetPawn());
	if (Enemy->GetIsActivatingConsumableItem()) return; // 현재 ExecuteTask에서 발동한 ConsumableItem 활성화 중

	if (!ExecutedConsumableItemMap.Contains(Enemy))
	{
		UC_Util::Print("From UC_BTTaskStatCare TickTask : ExecutedItem Map does not contains Enemy key!", GAMESCENE_MANAGER->GetTickRandomColor(), 10.f);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (!IsValid(ExecutedConsumableItemMap[Enemy]))
	{
		// 이미 Destroy된 아이템이라면 사용이 잘 완료되었다는 뜻
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		ExecutedConsumableItemMap.Remove(Enemy);
		return;
	}
	
	// 활성화 끝, 또는 활성화 도중 취소되었을 때
	EConsumableItemState ExecutedItemState = ExecutedConsumableItemMap[Enemy]->GetConsumableItemState();

	// 효과 활성화 중(ex) 구급상자나 붕대의 경우 회복량을 바로 회복하는 것이 아닌, 일정시간 나눠서 회복함)
	if (ExecutedItemState == EConsumableItemState::ACTIVATE_COMPLETED) return;
	
	ExecutedConsumableItemMap.Remove(Enemy);

	if (ExecutedItemState == EConsumableItemState::USED) UC_Util::Print("Consumable Used checked in BTTASK", GAMESCENE_MANAGER->GetTickRandomColor(), 10.f);

	FinishLatentTask(OwnerComp, (ExecutedItemState == EConsumableItemState::USED) ? EBTNodeResult::Succeeded : EBTNodeResult::Failed);

	Controller->GetBehaviorComponent()->SetIdleTaskType(EIdleTaskType::WAIT); // 다시 Wait 상황으로 돌아가기
}

EBTNodeResult::Type UC_BTTaskStatCare::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetOwner());
	if (!IsValid(Controller))
	{
		UC_Util::Print("From UC_BTTaskStatCare ExecuteTask : Controller Casting failed!", GAMESCENE_MANAGER->GetTickRandomColor(), 10.f);
		return EBTNodeResult::Failed;
	}

	AC_Enemy*				Enemy				= Cast<AC_Enemy>(Controller->GetPawn());
	UC_StatComponent*		EnemyStatComponent	= Enemy->GetStatComponent();
	UC_InvenComponent*		EnemyInvenComponent	= Enemy->GetInvenComponent();
	
	// 처리 우선순위 HP > Boost -> HP 우선적으로 회복 시도
	
	if (EnemyStatComponent->GetCurHP() < 100.f &&
		TryUsingHealItem(Enemy, EnemyInvenComponent)) return EBTNodeResult::InProgress;

	if (TryUsingBoostItem(Enemy, EnemyInvenComponent)) return EBTNodeResult::InProgress;
	
	return EBTNodeResult::Failed;
}

bool UC_BTTaskStatCare::TryUsingHealItem(AC_Enemy* Enemy, UC_InvenComponent* EnemyInvenComponent)
{
	// 피가 충분하다고 판단 처리
	if (Enemy->GetStatComponent()->GetCurHP() > 50.f) return false;
	
	// 처리 우선순위(MedKit, First Aid Kit, Bandage 순으로 회복 시도)
	FName MedKitItemName = AC_ConsumableItem::GetConsumableItemName(EConsumableItemType::MEDKIT);
	AC_ConsumableItem* MedKit = Cast<AC_ConsumableItem>(EnemyInvenComponent->FindMyItemByName(MedKitItemName));

	// MedKit이 있고 사용 시도 성공 했다면 return true
	if (IsValid(MedKit)) if (MedKit->StartUsingConsumableItem(Enemy))
	{
		ExecutedConsumableItemMap.Add(Enemy, MedKit);
		return true;
	}

	// First aid kit 사용 시도
	FName FirstAidKitItemName = AC_ConsumableItem::GetConsumableItemName(EConsumableItemType::FIRST_AID_KIT);
	AC_ConsumableItem* FirstAidKit = Cast<AC_ConsumableItem>(EnemyInvenComponent->FindMyItemByName(FirstAidKitItemName));
	if (IsValid(FirstAidKit)) if (FirstAidKit->StartUsingConsumableItem(Enemy))
	{
		ExecutedConsumableItemMap.Add(Enemy, FirstAidKit);
		return true;
	}

	// Bandage 사용 시도
	FName BandageItemName = AC_ConsumableItem::GetConsumableItemName(EConsumableItemType::BANDAGE);
	AC_ConsumableItem* Bandage = Cast<AC_ConsumableItem>(EnemyInvenComponent->FindMyItemByName(BandageItemName));
	if (IsValid(Bandage)) if (Bandage->StartUsingConsumableItem(Enemy))
	{
		ExecutedConsumableItemMap.Add(Enemy, Bandage);
		return true;
	}
	return false;
}

bool UC_BTTaskStatCare::TryUsingBoostItem(AC_Enemy* Enemy, UC_InvenComponent* EnemyInvenComponent)
{
	// 이미 충분히 Boosting 되어있다고 판단
	if (Enemy->GetStatComponent()->GetCurBoosting() > 50.f)
	{
		UC_Util::Print("From Stat Care Task : Try using boost item failed due to insufficient boost amount", GAMESCENE_MANAGER->GetTickRandomColor());
		return false;
	}
	
	FName DrinkItemName      = AC_ConsumableItem::GetConsumableItemName(EConsumableItemType::ENERGY_DRINK);
	FName PainKillerItemName = AC_ConsumableItem::GetConsumableItemName(EConsumableItemType::PAIN_KILLER);

	AC_ConsumableItem* EnergyDrink = Cast<AC_ConsumableItem>(EnemyInvenComponent->FindMyItemByName(DrinkItemName));
	AC_ConsumableItem* PainKiller  = Cast<AC_ConsumableItem>(EnemyInvenComponent->FindMyItemByName(PainKillerItemName));

	if (IsValid(EnergyDrink) && IsValid(PainKiller))
	{
		// 둘 다 Inven에 존재할 때에는 랜덤하게 진통제 & 에너지 드링크 중 택 1
		if (FMath::RandRange(0, 1) && PainKiller->StartUsingConsumableItem(Enemy))
		{
			ExecutedConsumableItemMap.Add(Enemy, PainKiller);
			return true;
		}

		if (EnergyDrink->StartUsingConsumableItem(Enemy))
		{
			ExecutedConsumableItemMap.Add(Enemy, EnergyDrink);
			return true;
		}
		return false;
	}

	// 둘 중 하나만 존재할 때
	if (IsValid(EnergyDrink))
	{
		if (EnergyDrink->StartUsingConsumableItem(Enemy))
		{
			ExecutedConsumableItemMap.Add(Enemy, EnergyDrink);
			return true;
		}
		return false;
	}
	
	if (IsValid(PainKiller)) if (PainKiller->StartUsingConsumableItem(Enemy))
	{
		ExecutedConsumableItemMap.Add(Enemy, PainKiller);
		return true;
	}

	// 아무 Boost item도 없거나 이전 PainKiller 사용 실패
	return false;  
}
