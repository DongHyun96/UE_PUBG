// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equipment/C_Helmet.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_InvenSystem.h"
#include "HUD/C_ArmorInfoWidget.h"
#include "HUD/C_HUDWidget.h"
#include "Utility/C_Util.h"


AC_Helmet::AC_Helmet()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_Helmet::BeginPlay()
{
	Super::BeginPlay();

	HelmetMesh = FindComponentByClass<UStaticMeshComponent>();

	//if (!IsValid(HelmetMesh))
	//	UC_Util::Print("From AC_Helmet::Beginplay : HelmetMesh var not inited!", FColor::Red, 10.f);
}

bool AC_Helmet::Interaction(AC_BasicCharacter* Character)
{
	Character->GetInvenComponent()->SetSlotEquipment(EEquipSlot::HELMET, this);
	return true;
}

bool AC_Helmet::LegacyMoveToAround(AC_BasicCharacter* Character)
{
	Character->GetInvenComponent()->SetSlotEquipment(EEquipSlot::HELMET, nullptr);
	OwnerCharacter = nullptr;

	return false;
}

bool AC_Helmet::LegacyMoveToSlot(AC_BasicCharacter* Character)
{
	//TODO : 장착되어 있던 Vest 처리해주기 및 제대로 구현하기, SetSlotEquipment함수 참고하기.(TODO From Vest)
	Character->GetInvenComponent()->SetSlotEquipment(EEquipSlot::HELMET, this);
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
	//BackpackMesh->SetVisibility(true);

	//DrawDebugSphere(GetWorld(), InParent->GetSocketLocation(SocketName), 10.f, 12, FColor::Red, false, 10.f);

	SetOwnerCharacter(InParent);

	// Player의 경우 HUD Armor Info 업데이트 해주기
	if (AC_Player* Player = Cast<AC_Player>(InParent))
		Player->GetHUDWidget()->GetArmorInfoWidget()->SetHelmetInfo(static_cast<uint8>(ItemLevel) + 1, CurDurability / DURABILITY_MAX);

	if (!Attached) UC_Util::Print("Not Attached", FColor::Cyan, 5.f);
	
}

bool AC_Helmet::MoveSlotToAround(AC_BasicCharacter* Character)
{
	Character->GetInvenComponent()->SetSlotEquipment(EEquipSlot::HELMET, nullptr);
	//OwnerCharacter = nullptr;
	this->SetItemPlace(EItemPlace::AROUND);

	// Player의 경우 HUD Armor info 업데이트
	if (AC_Player* Player = Cast<AC_Player>(Character))
		Player->GetHUDWidget()->GetArmorInfoWidget()->SetHelmetInfo(0);
	
	return true;
}


bool AC_Helmet::MoveAroundToSlot(AC_BasicCharacter* Character)
{
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


	// OwnerCharacter가 Player인 경우, UI 업데이트
	if (AC_Player* Player = Cast<AC_Player>(OwnerCharacter))
	{
		// TODO : Inven UI의 헬멧 피도 업데이트 시키기
		
		if (CurDurability > 0.f)
		{
			Player->GetHUDWidget()->GetArmorInfoWidget()->SetCurrentHelmetDurabilityRate(CurDurability / DURABILITY_MAX);
			//UpdateDurabilityBar();
		}
		else
			Player->GetHUDWidget()->GetArmorInfoWidget()->SetHelmetInfo(0);
		Player->GetInvenSystem()->InitializeList();
	}
	
	// TODO : 이 라인 if문에 넣어주기
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorEnableCollision(true);
	HelmetMesh->SetSimulatePhysics(true);
	HelmetMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	HelmetMesh->AddImpulse(FMath::VRand() * 2000.f);
	
	// Item의 DropItem은 OwnerCharacter가 nullptr일 경우, 처리를 안함 -> DropItem 내에서 위치 조정하는 부분을 넘기면서 SlotEquipment 세팅
	AC_BasicCharacter* PrevOwnerCharacter = OwnerCharacter;
	SetOwnerCharacter(nullptr);
	PrevOwnerCharacter->GetInvenComponent()->SetSlotEquipment(EEquipSlot::HELMET, nullptr);
	Cast<AC_Player>(PrevOwnerCharacter)->GetHUDWidget()->GetArmorInfoWidget()->SetHelmetInfo(0); // TODO : 이 라인은 없애고 if문에 넣기
	
	if (CurDurability < 0.f)
	{
		CurDurability = 0.f;
		
		// TODO : Helmet 날리기
	}

	return true;
}

float AC_Helmet::GetDamageReduceFactor() const
{
	return	(ItemLevel == EEquipableItemLevel::LV1) ? 0.7f :
			(ItemLevel == EEquipableItemLevel::LV2) ? 0.6f : 0.45f;
}
