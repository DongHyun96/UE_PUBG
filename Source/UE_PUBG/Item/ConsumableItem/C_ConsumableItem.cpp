// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem/C_ConsumableItem.h"
#include "Character/C_Player.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Item/Weapon/C_Weapon.h"
#include "Utility/C_Util.h"

AC_ConsumableItem::AC_ConsumableItem()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_ConsumableItem::BeginPlay()
{
	Super::BeginPlay();
}

void AC_ConsumableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UsingTimer += DeltaTime;

	switch (ConsumableItemState)
	{
	case EConsumableItemState::IDLE:
		UsingTimer = 0.f;
		return;
	case EConsumableItemState::ACTIVATING:
	{

		if (UsingTimer < UsageTime)
		{
			HandleActivatingState(); // Pure virtual Template method

			//방해 받았는지 체크해서 방해를 받았다면 Activating Cancel 시키기
			UAnimInstance* UserAnimInstance = ItemUser->GetMesh()->GetAnimInstance();

			for (auto& Pair : UsingMontageMap)
				if (UserAnimInstance->Montage_IsPlaying(Pair.Value.AnimMontage)) return;

			// 방해를 받음
			CancelActivating();

			return;
		}

		// Activating end

		if (AC_Player* Player = Cast<AC_Player>(ItemUser))
			Player->OnConsumableItemActivatingEnd();

		ConsumableItemState = EConsumableItemState::ACTIVATE_COMPLETED;
		UsingTimer = 0.f;

		if (ItemUser->GetEquippedComponent()->GetCurWeapon()) // 착용 중인 무기가 있었을 때
		{
			UC_EquippedComponent* UserEquippedComponent = ItemUser->GetEquippedComponent();

			ItemUser->GetEquippedComponent()->SetNextWeaponType(UserEquippedComponent->GetCurWeaponType());
			FPriorityAnimMontage DrawMontage = UserEquippedComponent->GetCurWeapon()->GetCurDrawMontage();
			ItemUser->PlayAnimMontage(DrawMontage);
		}

		ItemUser->SetIsActivatingConsumableItem(false);
	}
		return;
	case EConsumableItemState::ACTIVATE_COMPLETED:
		HandleActivateCompletedState(); // Template Method
		return;
	case EConsumableItemState::USED:
	{
		if (AC_Player* Player = Cast<AC_Player>(ItemUser)) Player->OnConsumableUsed();

		// TODO : 아이템 삭제
		// TODO : 이 라인 지우기
		ConsumableItemState = EConsumableItemState::IDLE;
	}
		return;
	default:
		return;
	}
}

bool AC_ConsumableItem::CancelActivating()
{
	if (ConsumableItemState != EConsumableItemState::ACTIVATING) return false;

	if (AC_Player* Player = Cast<AC_Player>(ItemUser))
		Player->OnCancelActivatingConsumableItem();

	UAnimInstance* UserAnimInstance = ItemUser->GetMesh()->GetAnimInstance();

	for (auto& Pair : UsingMontageMap)
	{
		if (UserAnimInstance->Montage_IsPlaying(Pair.Value.AnimMontage))
		{
			UserAnimInstance->Montage_Stop(0.2f, Pair.Value.AnimMontage);
			break;
		}
	}

	//OwnerCharacter->PlayAnimMontage(ThrowWeapon->GetCurDrawMontage());
	if (ItemUser->GetEquippedComponent()->GetCurWeapon()) // 착용 중인 무기가 있었을 때
	{
		UC_EquippedComponent* UserEquippedComponent = ItemUser->GetEquippedComponent();

		ItemUser->GetEquippedComponent()->SetNextWeaponType(UserEquippedComponent->GetCurWeaponType());
		FPriorityAnimMontage DrawMontage = UserEquippedComponent->GetCurWeapon()->GetCurDrawMontage();
		ItemUser->PlayAnimMontage(DrawMontage);
	}

	ItemUser->SetIsActivatingConsumableItem(false);

	ConsumableItemState = EConsumableItemState::IDLE;
	UsingTimer			= 0.f;
	ItemUser			= nullptr;

	return true;
}
