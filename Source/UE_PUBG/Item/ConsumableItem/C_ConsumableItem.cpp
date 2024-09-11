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

			//방해 받았는지 체크해서 방해를 받았다면 Activating Cancel 시키기

			if (ItemUser->GetCharacterMovement()->IsFalling())
			{
				CancelActivating();
				return;
			}

			UAnimInstance* UserAnimInstance = ItemUser->GetMesh()->GetAnimInstance();

			for (auto& Pair : UsingMontageMap) if (UserAnimInstance->Montage_IsPlaying(Pair.Value.AnimMontage)) return;

			// 방해를 받음
			CancelActivating();

			return;
		}

		// Activating end

		if (AC_Player* Player = Cast<AC_Player>(ItemUser))
			Player->GetHUDWidget()->OnConsumableItemActivatingEnd();

		ConsumableItemState = EConsumableItemState::ACTIVATE_COMPLETED;
		UsingTimer = 0.f;

		if (ItemUser->GetEquippedComponent()->GetCurWeapon()) // 착용 중인 무기가 있었을 때
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

		// TODO : 아이템 삭제 (몇 초 뒤에 삭제처리해야함) -> 붕대의 UsageMesh 없애는 시간까지는 기다려야 함
		// TODO : Inventory UI에서 지우기
		// TODO : 이 라인 지우기
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

	// 다른 Montage에 의해 방해 받았을 때
	if (PlayTime == 0.f) return false;

	// 사용 시작하기
	ConsumableItemState = EConsumableItemState::ACTIVATING;
	OnStartUsing(); // Template method


	// 현재 들고 있는 무기가 존재한다면 무기 잠깐 몸 쪽에 붙이기
	if (AC_Weapon* UserWeapon = ItemUser->GetEquippedComponent()->GetCurWeapon())
	{
		UserWeapon->AttachToHolster(ItemUser->GetMesh());
		ItemUser->SetHandState(EHandState::UNARMED);

		// 총기류 예외처리
		//if (AC_Gun* Gun = Cast<AC_Gun>(UserWeapon)) // 현재 들고 있는 무기가 총기라면
		//	Gun->SetIsAimPress(false);				//총을 들고 Aiming 중일 때 카메라 다시 원래대로 전환
	}

	// 사용자의 bIsActivatingConsumableItem 세팅
	ItemUser->SetIsActivatingConsumableItem(true);

	return true;
}

bool AC_ConsumableItem::CancelActivating()
{
	// TODO : 한 캐릭터 안에서 통용된 CancelActivating으로 수정해야 함

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

	if (ItemUser->GetEquippedComponent()->GetCurWeapon()) // 착용 중인 무기가 있었을 때
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
