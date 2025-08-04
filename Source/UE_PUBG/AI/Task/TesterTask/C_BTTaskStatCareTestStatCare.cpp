// Fill out your copyright notice in the Description page of Project Settings.


#include "C_BTTaskStatCareTestStatCare.h"

#include "Character/C_Enemy.h"
#include "Character/Component/C_InvenComponent.h"
#include "Item/ConsumableItem/C_ConsumableItem.h"
#include "Utility/C_Util.h"

bool UC_BTTaskStatCareTestStatCare::TryUsingHealItem(AC_Enemy* Enemy, UC_InvenComponent* EnemyInvenComponent)
{
	UC_StatComponent* EnemyStatComponent = Enemy->GetStatComponent();
	
	// 피가 충분하다고 판단 처리
	if (EnemyStatComponent->GetCurHP() > 75.f) return false;

	// 피가 25 밑일 때에는 무조건 MedKit 사용 시도 처리
	if (EnemyStatComponent->GetCurHP() < 25.f && TryUseMedKit(Enemy, EnemyInvenComponent))
	{
		EnemyStatComponent->SetCurBoosting(0.f); // For Testing : TODO : 이 라인 지우기
		return true;
	}

	// MedKit과 FirstAidKit 양자택일
	if (FMath::RandRange(0, 1)) if (TryUseMedKit(Enemy, EnemyInvenComponent))
	{
		EnemyStatComponent->SetCurBoosting(0.f); // For Testing : TODO : 이 라인 지우기
		return true;
	}
	
	// First aid kit 사용 시도
	FName FirstAidKitItemName = AC_ConsumableItem::GetConsumableItemName(EConsumableItemType::FIRST_AID_KIT);
	AC_ConsumableItem* FirstAidKit = Cast<AC_ConsumableItem>(EnemyInvenComponent->FindMyItemByName(FirstAidKitItemName));
	if (IsValid(FirstAidKit)) if (FirstAidKit->StartUsingConsumableItem(Enemy))
	{
		ExecutedConsumableItemMap.Add(Enemy, FirstAidKit);
		EnemyStatComponent->SetCurBoosting(0.f); // For Testing : TODO : 이 라인 지우기
		return true;
	}
	
	return false;
}

bool UC_BTTaskStatCareTestStatCare::TryUsingBoostItem(AC_Enemy* Enemy, UC_InvenComponent* EnemyInvenComponent)
{
	// 60 이상으로 Boost Gauge가 채워져 있다면 충분히 Boosting 되었다고 판단
	if (Enemy->GetStatComponent()->GetCurBoosting() > 60.f) return false;

	FName DrinkItemName      = AC_ConsumableItem::GetConsumableItemName(EConsumableItemType::ENERGY_DRINK);
	FName PainKillerItemName = AC_ConsumableItem::GetConsumableItemName(EConsumableItemType::PAIN_KILLER);

	AC_ConsumableItem* EnergyDrink = Cast<AC_ConsumableItem>(EnemyInvenComponent->FindMyItemByName(DrinkItemName));
	AC_ConsumableItem* PainKiller  = Cast<AC_ConsumableItem>(EnemyInvenComponent->FindMyItemByName(PainKillerItemName));

	if (!EnergyDrink || !PainKiller)
	{
		UC_Util::Print("From UC_BTTaskStatCareTestStatCare::TryUsingBoostItem : Drink or PainKiller not valid!, both should be valid", FColor::Red, 10.f);
		return false;
	}

	// EnergyDrink와 PainKiller 양자택일
	if (FMath::RandRange(0, 1)) if (PainKiller->StartUsingConsumableItem(Enemy))
	{
		ExecutedConsumableItemMap.Add(Enemy, PainKiller);
		return true;
	}
	
	// Try using Energy Drink
	if (EnergyDrink->StartUsingConsumableItem(Enemy))
	{
		ExecutedConsumableItemMap.Add(Enemy, EnergyDrink);
		return true;
	}
	
	return false;
}

bool UC_BTTaskStatCareTestStatCare::TryUseMedKit(AC_Enemy* Enemy, UC_InvenComponent* EnemyInvenComponent)
{
	FName MedKitItemName = AC_ConsumableItem::GetConsumableItemName(EConsumableItemType::MEDKIT);
	AC_ConsumableItem* MedKit = Cast<AC_ConsumableItem>(EnemyInvenComponent->FindMyItemByName(MedKitItemName));
		
	if (!MedKit)
	{
		UC_Util::Print("From UC_BTTaskStatCareTestStatCare::TryUsingHealItem : Invalid MedKit!", FColor::Red, 10.f);
		return false;
	}

	// MedKit이 있긴 하지만, 사용할 수 없는 상황일 경우
	if (!MedKit->StartUsingConsumableItem(Enemy)) return false;

	ExecutedConsumableItemMap.Add(Enemy, MedKit);
	return true;
}
