// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equipment/C_Helmet.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_InvenSystem.h"
#include "HUD/C_ArmorInfoWidget.h"
#include "HUD/C_HUDWidget.h"
//#include "InvenUserInterface/C_EquipmentPanel.h"
#include "Utility/C_Util.h"


AC_Helmet::AC_Helmet()
{
	//PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bCanEverTick = false;

}

void AC_Helmet::BeginPlay()
{
	Super::BeginPlay();

	HelmetMesh = FindComponentByClass<UStaticMeshComponent>();

}

bool AC_Helmet::Interaction(AC_BasicCharacter* Character)
{
	MoveToSlot(Character, GetItemCurStack());
	return true;
}

void AC_Helmet::AttachToSocket(class AC_BasicCharacter* InParent)
{
	SocketName = (ItemLevel == EEquipableItemLevel::LV1) ? "HelmetLv1Socket" :
				 (ItemLevel == EEquipableItemLevel::LV2) ? "HelmetLv2Socket" : "HelmetLv3Socket";

	bool Attached = AttachToComponent
	(
		InParent->GetMesh(),
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		SocketName
	);

	SetOwnerCharacter(InParent);

	// Player의 경우 HUD Armor Info 업데이트 해주기
	if (AC_Player* Player = Cast<AC_Player>(InParent))
		Player->GetHUDWidget()->GetArmorInfoWidget()->SetHelmetInfo(static_cast<uint8>(ItemLevel) + 1, CurDurability / DURABILITY_MAX);

	if (!Attached) UC_Util::Print("Not Attached", FColor::Cyan, 5.f);
	
}

bool AC_Helmet::MoveSlotToAround(AC_BasicCharacter* Character, int32 InStack)
{
	Character->GetInvenComponent()->SetSlotEquipment(EEquipSlot::HELMET, nullptr);
	//OwnerCharacter = nullptr;
	this->SetItemPlace(EItemPlace::AROUND);

	// Player의 경우 HUD Armor info 업데이트
	if (AC_Player* Player = Cast<AC_Player>(Character))
		Player->GetHUDWidget()->GetArmorInfoWidget()->SetHelmetInfo(0);
	
	return true;
}


bool AC_Helmet::MoveAroundToSlot(AC_BasicCharacter* Character, int32 InStack)
{
	if (AC_EquipableItem* curHelmet = Character->GetInvenComponent()->GetEquipmentItems()[EEquipSlot::HELMET])
		curHelmet->MoveToAround(Character, InStack);
	Character->GetInvenComponent()->SetSlotEquipment(EEquipSlot::HELMET, this);
	return true;
}

bool AC_Helmet::TakeDamage(float DamageAmount)
{
	// Level에 따른 Helmet에 적용되는 Damage량 조절
	float DamageReduceRate =	(ItemLevel == EEquipableItemLevel::LV1) ? 1.25f  :
								(ItemLevel == EEquipableItemLevel::LV2) ? 0.666f : 0.434f;
	DamageAmount *= DamageReduceRate;
	
	CurDurability -= DamageAmount;

	if (CurDurability < 0.f) CurDurability = 0.f;

	if (CurDurability == 0.f)
	{
		// 이 헬멧 날리기
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		SetActorEnableCollision(true);
		HelmetMesh->SetSimulatePhysics(true);
		HelmetMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		HelmetMesh->AddImpulse(FMath::VRand() * 2000.f);
		
		// Item의 DropItem은 OwnerCharacter가 nullptr일 경우, 처리를 안함 -> DropItem 내에서 위치 조정하는 부분을 넘기면서 SlotEquipment 세팅
		AC_BasicCharacter* PrevOwnerCharacter = OwnerCharacter;
		SetOwnerCharacter(nullptr);
		if (PrevOwnerCharacter)
		{
			PrevOwnerCharacter->GetInvenComponent()->SetSlotEquipment(EEquipSlot::HELMET, nullptr);
			if (AC_Player* Player = Cast<AC_Player>(PrevOwnerCharacter))
				Player->GetInvenSystem()->InitializeList();
		}
		this->SetItemPlace(EItemPlace::NONE);
		return true;
	}
	
	// OwnerCharacter가 Player인 경우, UI 업데이트
	if (AC_Player* Player = Cast<AC_Player>(OwnerCharacter))
	{
		Player->GetHUDWidget()->GetArmorInfoWidget()->SetCurrentHelmetDurabilityRate(CurDurability / DURABILITY_MAX);
		Player->GetInvenSystem()->InitializeList();
	}

	
	return true;
}

float AC_Helmet::GetDamageReduceFactor() const
{
	return	(ItemLevel == EEquipableItemLevel::LV1) ? 0.7f :
			(ItemLevel == EEquipableItemLevel::LV2) ? 0.6f : 0.45f;
}
