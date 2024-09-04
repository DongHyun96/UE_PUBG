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
	// �̹� �ν��� �ִ�ġ�� ��
	if (InItemUser->GetStatComponent()->GetCurBoosting() >= 100.f) return false;
	if (ConsumableItemState != EConsumableItemState::IDLE) return false;

	ItemUser = InItemUser;

	if (!UsingMontageMap.Contains(ItemUser->GetPoseState()))    return false;
	if (!UsingMontageMap[ItemUser->GetPoseState()].AnimMontage) return false;

	float PlayTime = ItemUser->PlayAnimMontage(UsingMontageMap[ItemUser->GetPoseState()]);
	if (PlayTime == 0.f) return false; // �ٸ� ��Ÿ�ֿ� ���� ������ ������ �ȵǾ��� ��

	UC_Util::Print("Starts to use Energy Drink!");

	// ��� �����ϱ�
	ConsumableItemState = EConsumableItemState::ACTIVATING;
	UsingTimer			= 0.f;
	UsageTime			= 4.f;

	if (ItemUser->GetEquippedComponent()->GetCurWeapon()) // ���� ��� �ִ� ���Ⱑ �����Ѵٸ� ���� ��� �� �ʿ� ���̱�
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
