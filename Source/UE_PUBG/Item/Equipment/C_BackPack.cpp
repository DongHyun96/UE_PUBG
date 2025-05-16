// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Equipment/C_BackPack.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_InvenSystem.h"
#include "InvenUI/C_InventoryUIWidget.h"
#include "Components/SphereComponent.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Utility/C_Util.h"
#include "HUD/C_ArmorInfoWidget.h"
#include "HUD/C_HUDWidget.h"


AC_BackPack::AC_BackPack()
{
	//PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bCanEverTick = false;

	Level = 3;

}

void AC_BackPack::BeginPlay()
{
	Super::BeginPlay();
}

void AC_BackPack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AC_BackPack::AttachToSocket(AC_BasicCharacter* InParent)
{
	//FName SocketName{};
	switch (Level)
	{
	case 1:
		SocketName = "BackPack_Lv1";
		break;
	case 2:
		SocketName = "BackPack_Lv2";
		break;
	case 3:
		SocketName = "BackPack_Lv3";
		break;
	default:
		break;
	}
	
	//mobility가 Moveable이여야 장착 가능. 지금은 에디터 내에서 설정해 놓았음.
	//BackpackMesh->SetMobility(EComponentMobility::Movable);

	bool Attached = AttachToComponent
	(
		InParent->GetMesh(),
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		SocketName
	);
	//BackpackMesh->SetVisibility(true);

	//DrawDebugSphere(GetWorld(), InParent->GetSocketLocation(SocketName), 10.f, 12, FColor::Red, false, 10.f);

	SetOwnerCharacter(InParent);

	// Player일 경우, HUD Armor info widget 업데이트
	if (AC_Player* Player = Cast<AC_Player>(InParent))
	{
		float CurMaxVolume	= Player->GetInvenComponent()->GetMaxVolume();
		float CurVolume		= Player->GetInvenComponent()->GetCurVolume();
		Player->GetHUDWidget()->GetArmorInfoWidget()->SetBackPackInfo(static_cast<uint8>(ItemLevel) + 1, CurVolume / CurMaxVolume);
	}
	
	if (!Attached) UC_Util::Print("Not Attached", FColor::Cyan, 5.f);
}

bool AC_BackPack::Interaction(AC_BasicCharacter* Character)
{
	//PickUpItem(Character);
	switch (ItemPlace)
	{
	case EItemPlace::AROUND:
		//return LegacyMoveToSlot(Character);
		return MoveToSlot(Character, GetItemCurStack());
	case EItemPlace::SLOT:
		//return LegacyMoveToAround(Character);
		return MoveToAround(Character, GetItemCurStack());
	default:
		return false;
	}
	//return false;//Character->GetInvenComponent()->CheckMyBackPack(this);
}

bool AC_BackPack::MoveSlotToAround(AC_BasicCharacter* Character,int32 InStack)
{
	
	UC_InvenComponent* InvenComp = Character->GetInvenComponent();
	AC_BackPack* curBackPack = nullptr;
	curBackPack = Cast<AC_BackPack>(InvenComp->GetEquipmentItems()[EEquipSlot::BACKPACK]);

	float curVolume = InvenComp->GetCurVolume();
	float preMaxVolume = 70.f + InvenComp->CheckBackPackVolume(this->GetLevel()) + InvenComp->GetVestVolume();

	if (curVolume > preMaxVolume) return false;

	if (curBackPack)
	{
		//curBackPack->SetOwnerCharacter(nullptr);
		InvenComp->SetSlotEquipment(EEquipSlot::BACKPACK, nullptr);
		//InvenComp->RemoveBackPack();
		//curBackPack->DetachItem();
	}

	InvenComp->CheckBackPackOnCharacter();

	this->SetItemPlace(EItemPlace::AROUND);

	// Player일 경우, Armor info widget 업데이트
	if (AC_Player* Player = Cast<AC_Player>(Character))
		Player->GetHUDWidget()->GetArmorInfoWidget()->SetBackPackInfo(0);

	return true;
}

bool AC_BackPack::MoveAroundToSlot(AC_BasicCharacter* Character, int32 InStack)
{

	
	//TODO : PickUpItem 내용으로 우선 구현한 것. 다시 구현하기.
	//캐릭터의 현재 용량과 바꾼 가방의 최대용량을 비교해서 바꾸기.
	UC_InvenComponent* InvenComp = Character->GetInvenComponent();
	UC_EquippedComponent* EquipComp = Character->GetEquippedComponent();

	AC_BackPack* curBackPack = nullptr;
	curBackPack = Cast<AC_BackPack>(InvenComp->GetEquipmentItems()[EEquipSlot::BACKPACK]);

	float curVolume = InvenComp->GetCurVolume();
	float preMaxVolume = 70.f + InvenComp->CheckBackPackVolume(this->GetLevel()) + InvenComp->GetVestVolume();

	if (curVolume >= preMaxVolume) return false;

	if (curBackPack)
		curBackPack->MoveToAround(Character, InStack);

	//InvenComp->EquippedBackPack(this);

	InvenComp->SetSlotEquipment(EEquipSlot::BACKPACK, this);

	InvenComp->CheckBackPackOnCharacter();

	//this->SetItemPlace(EItemPlace::SLOT);

	return true;
}


