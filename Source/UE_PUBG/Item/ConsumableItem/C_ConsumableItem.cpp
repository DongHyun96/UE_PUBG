// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem/C_ConsumableItem.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Character/C_Player.h"
#include "HUD/C_HUDWidget.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_ConsumableUsageMeshComponent.h"
#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/Gun/C_Gun.h"
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

			//���� �޾Ҵ��� üũ�ؼ� ���ظ� �޾Ҵٸ� Activating Cancel ��Ű��

			if (ItemUser->GetCharacterMovement()->IsFalling())
			{
				CancelActivating();
				return;
			}

			UAnimInstance* UserAnimInstance = ItemUser->GetMesh()->GetAnimInstance();

			for (auto& Pair : UsingMontageMap) if (UserAnimInstance->Montage_IsPlaying(Pair.Value.AnimMontage)) return;

			// ���ظ� ����
			CancelActivating();

			return;
		}

		// Activating end

		if (AC_Player* Player = Cast<AC_Player>(ItemUser))
			Player->GetHUDWidget()->OnConsumableItemActivatingEnd();

		ConsumableItemState = EConsumableItemState::ACTIVATE_COMPLETED;
		UsingTimer = 0.f;

		if (ItemUser->GetEquippedComponent()->GetCurWeapon()) // ���� ���� ���Ⱑ �־��� ��
		{
			UC_EquippedComponent* UserEquippedComponent = ItemUser->GetEquippedComponent();

			ItemUser->GetEquippedComponent()->SetNextWeaponType(UserEquippedComponent->GetCurWeaponType());
			FPriorityAnimMontage DrawMontage = UserEquippedComponent->GetCurWeapon()->GetCurDrawMontage();
			ItemUser->PlayAnimMontage(DrawMontage);
		}

		OnActivatingFinish(); // Template method

		ItemUser->SetIsActivatingConsumableItem(false);
	}
		return;
	case EConsumableItemState::ACTIVATE_COMPLETED:
		HandleActivateCompletedState(); // Template Method
		return;
	case EConsumableItemState::USED:
	{
		if (AC_Player* Player = Cast<AC_Player>(ItemUser)) Player->GetHUDWidget()->OnConsumableUsed();

		// TODO : ������ ���� (�� �� �ڿ� ����ó���ؾ���) -> �ش��� UsageMesh ���ִ� �ð������� ��ٷ��� ��
		// TODO : Inventory UI���� �����
		// TODO : �� ���� �����
		ConsumableItemState = EConsumableItemState::IDLE;
	}
		return;
	default:
		return;
	}
}

bool AC_ConsumableItem::StartUsingConsumableItem(AC_BasicCharacter* InItemUser)
{
	if (!IsAvailableToStartUsing(InItemUser)) return false; // Template Method(IsAvaliableToStartUsing())
	if (ConsumableItemState != EConsumableItemState::IDLE) return false;

	ItemUser = InItemUser;

	if (ItemUser->GetIsActivatingConsumableItem())				return false;
	if (!UsingMontageMap.Contains(ItemUser->GetPoseState()))    return false;
	if (!UsingMontageMap[ItemUser->GetPoseState()].AnimMontage) return false;

	float PlayTime = ItemUser->PlayAnimMontage(UsingMontageMap[ItemUser->GetPoseState()]);

	// �ٸ� Montage�� ���� ���� �޾��� ��
	if (PlayTime == 0.f) return false;

	// ��� �����ϱ�
	ConsumableItemState = EConsumableItemState::ACTIVATING;
	OnStartUsing(); // Template method


	// ���� ��� �ִ� ���Ⱑ �����Ѵٸ� ���� ��� �� �ʿ� ���̱�
	if (AC_Weapon* UserWeapon = ItemUser->GetEquippedComponent()->GetCurWeapon())
	{
		UserWeapon->AttachToHolster(ItemUser->GetMesh());
		ItemUser->SetHandState(EHandState::UNARMED);

		// �ѱ�� ����ó��
		//if (AC_Gun* Gun = Cast<AC_Gun>(UserWeapon)) // ���� ��� �ִ� ���Ⱑ �ѱ���
		//	Gun->SetIsAimPress(false);				//���� ��� Aiming ���� �� ī�޶� �ٽ� ������� ��ȯ
	}

	// ������� bIsActivatingConsumableItem ����
	ItemUser->SetIsActivatingConsumableItem(true);

	return true;
}

bool AC_ConsumableItem::CancelActivating()
{
	// TODO : �� ĳ���� �ȿ��� ���� CancelActivating���� �����ؾ� ��

	if (ConsumableItemState != EConsumableItemState::ACTIVATING) return false;

	if (AC_Player* Player = Cast<AC_Player>(ItemUser))
		Player->GetHUDWidget()->OnCancelActivatingConsumableItem();

	UAnimInstance* UserAnimInstance = ItemUser->GetMesh()->GetAnimInstance();

	for (auto& Pair : UsingMontageMap)
	{
		if (UserAnimInstance->Montage_IsPlaying(Pair.Value.AnimMontage))
		{
			UserAnimInstance->Montage_Stop(0.2f, Pair.Value.AnimMontage);
			break;
		}
	}

	if (ItemUser->GetEquippedComponent()->GetCurWeapon()) // ���� ���� ���Ⱑ �־��� ��
	{
		UC_EquippedComponent* UserEquippedComponent = ItemUser->GetEquippedComponent();
		
		ItemUser->GetEquippedComponent()->SetNextWeaponType(UserEquippedComponent->GetCurWeaponType());
		FPriorityAnimMontage DrawMontage = UserEquippedComponent->GetCurWeapon()->GetCurDrawMontage();
		ItemUser->PlayAnimMontage(DrawMontage);
	}

	ItemUser->SetIsActivatingConsumableItem(false);
	OnCancelActivating();

	ConsumableItemState = EConsumableItemState::IDLE;
	UsingTimer			= 0.f;
	//ItemUser			= nullptr;

	return true;
}
