// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem/Healing/C_Bandage.h"
#include "Utility/C_Util.h"

#include "Character/C_Player.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Item/Weapon/C_Weapon.h"

AC_Bandage::AC_Bandage()
{
	PrimaryActorTick.bCanEverTick = true;

	UsageTime = 4.f;
}

void AC_Bandage::BeginPlay()
{
	Super::BeginPlay();
}

void AC_Bandage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AC_Bandage::StartUsingConsumableItem(AC_BasicCharacter* InItemUser)
{
	// 최대로 채울 수 있는 체력보다 더 많을 때
	if (InItemUser->GetStatComponent()->GetCurHP() >= InItemUser->GetStatComponent()->GetHealUpLimit()) return false;
	if (ConsumableItemState != EConsumableItemState::IDLE) return false;

	ItemUser = InItemUser;

	if (!UsingMontageMap.Contains(ItemUser->GetPoseState()))    return false;
	if (!UsingMontageMap[ItemUser->GetPoseState()].AnimMontage) return false;
	
	float PlayTime = ItemUser->PlayAnimMontage(UsingMontageMap[ItemUser->GetPoseState()]);

	// 다른 Montage에 의해 방해 받았을 때
	if (PlayTime == 0.f) return false;

	UC_Util::Print("Starts to use Bandage");

	// 사용 시작하기
	ConsumableItemState = EConsumableItemState::ACTIVATING;
	UsingTimer			= 0.f;
	UsageTime			= 4.f;
	BlockUsed			= 0;

	if (ItemUser->GetEquippedComponent()->GetCurWeapon()) // 현재 들고 있는 무기가 존재한다면 무기 잠깐 몸 쪽에 붙이기
		ItemUser->GetEquippedComponent()->GetCurWeapon()->AttachToHolster(ItemUser->GetMesh());

	ItemUser->SetIsActivatingConsumableItem(true);

	return true;
}

void AC_Bandage::HandleActivatingState()
{
	// TODO : Update HUD UI Timer & Update HealDestIndicator bar
	if (AC_Player* Player = Cast<AC_Player>(ItemUser))
	{
		float HealUpDestHPValue = FMath::Clamp(Player->GetStatComponent()->GetCurHP() + 10.f, 0.f, 75.f);
		Player->OnActivatingHealUp(HealUpDestHPValue, UsageTime, UsingTimer);
	}
}

void AC_Bandage::HandleActivateCompletedState()
{
	float NewHP = FMath::Clamp
	(
		ItemUser->GetStatComponent()->GetCurHP() + ONE_BLOCK_HEAL_AMOUNT,
		0.f, ItemUser->GetStatComponent()->GetHealUpLimit()
	);

	if (UsingTimer < ONE_BLOCK_TIME)
	{
		if (AC_Player* Player = Cast<AC_Player>(ItemUser))
			Player->OnActivateHealUpCompletedTick(NewHP, UsingTimer / ONE_BLOCK_TIME);
		return;
	}

	// UsingTimer Block 시간 다 채웠을 때

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
