// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem/Healing/C_MedKit.h"
#include "Utility/C_Util.h"

#include "Character/C_Player.h"

#include "Character/Component/C_EquippedComponent.h"
#include "Item/Weapon/C_Weapon.h"

AC_MedKit::AC_MedKit()
{
	PrimaryActorTick.bCanEverTick = true;

	UsageTime = 8.f;
}

void AC_MedKit::BeginPlay()
{
	Super::BeginPlay();
}

void AC_MedKit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AC_MedKit::StartUsingConsumableItem(AC_BasicCharacter* InItemUser)
{
	// �ִ�� ä�� �� �ִ� ü�º��� �� ���� ��
	if (InItemUser->GetStatComponent()->GetCurHP() >= 100.f) return false;
	if (ConsumableItemState != EConsumableItemState::IDLE) return false;

	ItemUser = InItemUser;

	if (!UsingMontageMap.Contains(ItemUser->GetPoseState()))    return false;
	if (!UsingMontageMap[ItemUser->GetPoseState()].AnimMontage) return false;

	float PlayTime = ItemUser->PlayAnimMontage(UsingMontageMap[ItemUser->GetPoseState()]);
	if (PlayTime == 0.f) return false;

	UC_Util::Print("Starts to use MedKit!");

	// ��� �����ϱ�
	ConsumableItemState = EConsumableItemState::ACTIVATING;
	UsingTimer			= 0.f;
	UsageTime			= 8.f;
	BlockUsed			= 0;

	if (ItemUser->GetEquippedComponent()->GetCurWeapon()) // ���� ��� �ִ� ���Ⱑ �����Ѵٸ� ���� ��� �� �ʿ� ���̱�
		ItemUser->GetEquippedComponent()->GetCurWeapon()->AttachToHolster(ItemUser->GetMesh());

	ItemUser->SetIsActivatingConsumableItem(true);

	return true;
}

void AC_MedKit::HandleActivatingState()
{
	// TODO : Update HUD UI Timer & Update HealDestIndicator bar
	if (AC_Player* Player = Cast<AC_Player>(ItemUser)) 
		Player->OnActivatingHealUp(100.f, UsageTime, UsingTimer);
}

void AC_MedKit::HandleActivateCompletedState()
{
	const float NewHP = 100.f;

	if (UsingTimer < ONE_BLOCK_TIME)
	{
		if (AC_Player* Player = Cast<AC_Player>(ItemUser))
			Player->OnActivateHealUpCompletedTick(NewHP, UsingTimer / ONE_BLOCK_TIME);
		return;
	}

	// UsingTimer Block �ð� �� ä���� ��

	ItemUser->GetStatComponent()->SetCurHP(NewHP);
	BlockUsed++;
	UsingTimer -= ONE_BLOCK_TIME;

	if (BlockUsed >= TOTAL_USING_BLOCK_CNT ||
		ItemUser->GetStatComponent()->GetCurHP() >= ItemUser->GetStatComponent()->GetHealUpLimit()) // End of HandleActivateCompletedState
	{
		ConsumableItemState = EConsumableItemState::USED;
		return;
	}
}
