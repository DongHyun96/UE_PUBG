// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem/Boosting/C_EnergyDrink.h"
#include "Utility/C_Util.h"
#include "Character/C_Player.h"

#include "Character/Component/C_EquippedComponent.h"
#include "Item/Weapon/C_Weapon.h"

AC_EnergyDrink::AC_EnergyDrink()
{
	PrimaryActorTick.bCanEverTick = true;

	UsageTime = 4.f;
}

void AC_EnergyDrink::BeginPlay()
{
	Super::BeginPlay();
}

void AC_EnergyDrink::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AC_EnergyDrink::StartUsingConsumableItem(AC_BasicCharacter* InItemUser)
{
	// 이미 부스팅 최대치일 때
	if (InItemUser->GetStatComponent()->GetCurBoosting() >= 100.f) return false;
	if (ConsumableItemState != EConsumableItemState::IDLE) return false;

	ItemUser = InItemUser;

	if (!UsingMontageMap.Contains(ItemUser->GetPoseState()))    return false;
	if (!UsingMontageMap[ItemUser->GetPoseState()].AnimMontage) return false;

	float PlayTime = ItemUser->PlayAnimMontage(UsingMontageMap[ItemUser->GetPoseState()]);
	if (PlayTime == 0.f) return false; // 다른 몽타주에 의해 동작이 시작이 안되었을 때

	UC_Util::Print("Starts to use Energy Drink!");

	// 사용 시작하기
	ConsumableItemState = EConsumableItemState::ACTIVATING;
	UsingTimer			= 0.f;
	UsageTime			= 4.f;

	if (ItemUser->GetEquippedComponent()->GetCurWeapon()) // 현재 들고 있는 무기가 존재한다면 무기 잠깐 몸 쪽에 붙이기
		ItemUser->GetEquippedComponent()->GetCurWeapon()->AttachToHolster(ItemUser->GetMesh());

	ItemUser->SetIsActivatingConsumableItem(true);

	return true;
}

void AC_EnergyDrink::HandleActivatingState()
{
	// TODO : Update HUD UI Timer & Update HealDestIndicator bar
	if (AC_Player* Player = Cast<AC_Player>(ItemUser))
		Player->OnActivatingBooster(UsageTime, UsingTimer);
}

void AC_EnergyDrink::HandleActivateCompletedState()
{
	ItemUser->GetStatComponent()->AddBoost(BOOST_AMOUNT);
	UsingTimer = 0.f;
	ConsumableItemState = EConsumableItemState::USED;
}
