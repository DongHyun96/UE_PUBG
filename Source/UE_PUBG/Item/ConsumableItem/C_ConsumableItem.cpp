// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem/C_ConsumableItem.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Character/C_Player.h"
#include "HUD/C_HUDWidget.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_ConsumableUsageMeshComponent.h"
#include "Character/Component/C_SwimmingComponent.h"
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

			if (ItemUser->GetCharacterMovement()->IsFalling() || ItemUser->GetSwimmingComponent()->IsSwimming())
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

		ItemDatas.ItemStack--;

		if (ItemDatas.ItemStack == 0)
		{
			ItemUser->GetInvenComponent()->RemoveItemToMyList(this);
			HandleDestroy(); // �� ������ �� Destroy �ϴ� ������ �ٸ�
			return;
		}

		ConsumableItemState = EConsumableItemState::IDLE;
	}
		return;
	default:
		return;
	}
}

bool AC_ConsumableItem::StartUsingConsumableItem(AC_BasicCharacter* InItemUser)
{
	if (!IsAvailableToStartUsing(InItemUser))				return false; // Template Method(IsAvaliableToStartUsing())
	if (InItemUser->GetSwimmingComponent()->IsSwimming())	return false;
	if (ConsumableItemState != EConsumableItemState::IDLE)	return false;

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
		if (AC_Gun* Gun = Cast<AC_Gun>(UserWeapon)) Gun->BackToMainCamera();
	}

	// ������� bIsActivatingConsumableItem ����
	ItemUser->SetIsActivatingConsumableItem(true);

	//ItemDatas.ItemStack--;

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

bool AC_ConsumableItem::Interaction(AC_BasicCharacter* Character)
{

	switch (ItemDatas.ItemPlace)
	{
	case EItemPlace::AROUND:
		if (!OwnerCharacter) return MoveToInven(Character);
	case EItemPlace::INVEN:
		return StartUsingConsumableItem(Character);
		//break;
	default:
		return false;
	}
}

bool AC_ConsumableItem::MoveToInven(AC_BasicCharacter* Character)
{
	UC_InvenComponent* invenComp = Character->GetInvenComponent();
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();

	uint8 ItemStackCount = invenComp->LoopCheckVolume(this);

	if (ItemStackCount == 0)
	{
		UC_Util::Print("Not Enough Volume");
		return false; //�κ��� ���� �� �ִ� �������� ������ 0 �̸� ���� �� �����Ƿ� return false;
	}

	AC_Item* FoundItem = invenComp->FindMyItem(this); //�κ��� ���� �������� ã�ƿ�, ���ٸ� nullptr;

	if (ItemDatas.ItemStack == ItemStackCount)
	{
		//������ ���θ� �κ��� ���� �� ���� ��.
		if (IsValid(FoundItem))
		{
			//�κ��� �ش� �������� ���� �� ��.
			FoundItem->SetItemStack(FoundItem->GetItemDatas().ItemStack + ItemStackCount);
			//invenComp->GetCurVolume() += FoundItem->GetItemDatas().ItemVolume * ItemStackCount;
			//TODO : destroy�� �ص� �ܻ��� ���°��� ����ؼ� �س��� ���� ���̵� �ܻ��� �ȳ��´ٸ� ���� ��.
			invenComp->AddInvenCurVolume(this->ItemDatas.ItemVolume * ItemStackCount);

			this->SetActorEnableCollision(false);
			this->SetActorHiddenInGame(true);

			this->Destroy();
			return true;
		}
		else
		{
			//�κ��� �ش� �������� �������� ���� ��.
			invenComp->AddItemToMyList(this);
			//�ΰ��ӿ��� ���̴� �Ͱ� collision���� ������ �ӽ÷� ����.
			//this->SetActorEnableCollision(false);
			this->SetActorHiddenInGame(true);
			//���� �� ����. �̰ɷ� ���� �������� �������� �ȳ����ٸ� �ٸ� ��� ���->ToInven���� SetActorEnableCollision�� ���ְ� ������ Ȥ�� ToAround���� ���ֱ�.
			//Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			return true;
		}
	}
	else
	{
		//�������� �Ϻθ� �κ��� ���� �� ���� ��.
		if (IsValid(FoundItem))
		{
			this->SetItemStack(ItemDatas.ItemStack - ItemStackCount);
			FoundItem->SetItemStack(FoundItem->GetItemDatas().ItemStack + ItemStackCount);

			invenComp->AddInvenCurVolume(this->ItemDatas.ItemVolume * ItemStackCount);

			return true;
		}
		else
		{
			AC_Weapon* NewItem = Cast<AC_Weapon>(SpawnItem(Character));//������ ���� ����
			NewItem->SetItemStack(ItemStackCount);
			this->SetItemStack(ItemDatas.ItemStack - ItemStackCount);

			invenComp->AddItemToMyList(NewItem);

			NewItem->SetActorHiddenInGame(true);
			//collider ���� ���� �߰��ؾ� �� �� ����.
			//���� �߰��ؾ� �ȴٸ� MoveToInven���� SetActorEnableCollision�� ���ְ� ���� �� ���ִ� �������.
			return true;
		}
	}
}

bool AC_ConsumableItem::MoveToSlot(AC_BasicCharacter* Character)
{
	return false;
}

bool AC_ConsumableItem::MoveToAround(AC_BasicCharacter* Character)
{
	if (!Character) return false;

	Character->GetInvenComponent()->RemoveItemToMyList(this);
	//TODO: �����ؼ� ������ ��� ���� �������־����.
	ItemDatas.ItemPlace = EItemPlace::AROUND;
	SetOwnerCharacter(nullptr);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	//Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	//�ٴ� ���� ĳ���� �޾ƿͼ� �ٴڿ� ������ �����ϱ�.
	SetActorLocation(GetGroundLocation(Character) + RootComponent->Bounds.BoxExtent.Z);

	//SetActorRotation(FQuat(0,0,0));

	return true;
}
