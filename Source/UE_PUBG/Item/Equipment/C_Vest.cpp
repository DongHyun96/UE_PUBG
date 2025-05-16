// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equipment/C_Vest.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_InvenSystem.h"
#include "HUD/C_ArmorInfoWidget.h"
#include "HUD/C_HUDWidget.h"
#include "Utility/C_Util.h"

AC_Vest::AC_Vest()
{
	//PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bCanEverTick = false;

}

bool AC_Vest::Interaction(AC_BasicCharacter* Character)
{
	//Character->GetInvenComponent()->SetSlotEquipment(EEquipSlot::VEST, this);
	MoveToSlot(Character, GetItemCurStack());
	return true;
}

void AC_Vest::AttachToSocket(AC_BasicCharacter* InParent)
{
	//FName SocketName{};
	switch (ItemLevel)
	{
	case EEquipableItemLevel::LV1:
		SocketName = "Vest_1";
		break;
	case EEquipableItemLevel::LV2:
		SocketName = "Vest_2";
		break;
	case EEquipableItemLevel::LV3:
		SocketName = "Vest_3";
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

	// Player의 경우 HUD Armor Info 업데이트 해주기
	if (AC_Player* Player = Cast<AC_Player>(InParent))
		Player->GetHUDWidget()->GetArmorInfoWidget()->SetVestInfo(static_cast<uint8>(ItemLevel) + 1, CurDurability / DURABILITY_MAX);

	if (!Attached) UC_Util::Print("Not Attached", FColor::Cyan, 5.f);
}

bool AC_Vest::MoveSlotToAround(AC_BasicCharacter* Character,int32 InStack)
{
	Character->GetInvenComponent()->SetSlotEquipment(EEquipSlot::VEST, nullptr);
	//OwnerCharacter = nullptr;
	this->SetItemPlace(EItemPlace::AROUND);

	// Player의 경우 HUD Armor info 업데이트
	if (AC_Player* Player = Cast<AC_Player>(Character))
		Player->GetHUDWidget()->GetArmorInfoWidget()->SetVestInfo(0);
	
	return true;
}

bool AC_Vest::MoveAroundToSlot(AC_BasicCharacter* Character,int32 InStack)
{
	//TODO : 장착되어 있던 Vest 처리해주기 및 제대로 구현하기, SetSlotEquipment함수 참고하기.
	if (AC_EquipableItem* curVest = Character->GetInvenComponent()->GetEquipmentItems()[EEquipSlot::VEST])
		curVest->MoveToAround(Character, InStack);
	Character->GetInvenComponent()->SetSlotEquipment(EEquipSlot::VEST, this);
	return true;
}

bool AC_Vest::TakeDamage(float DamageAmount)
{
	// UC_Util::Print("AC_Vest::TakeDamage : " + FString::SanitizeFloat(DamageAmount), FColor::MakeRandomColor(), 10.f);
	
	// Level에 따른 Vest에 적용되는 Damage량 조절
	float DamageReduceRate =	(ItemLevel == EEquipableItemLevel::LV1) ? 0.5f  :
								(ItemLevel == EEquipableItemLevel::LV2) ? 0.45f : 0.4f;
	DamageAmount *= DamageReduceRate;
	
	CurDurability -= DamageAmount;


	if (CurDurability < 0.f) CurDurability = 0.f;


	// OwnerCharacter가 Player인 경우, UI 업데이트
	if (AC_Player* Player = Cast<AC_Player>(OwnerCharacter))
	{
		// TODO : Inven UI의 조끼 피도 업데이트 시키기
		Player->GetHUDWidget()->GetArmorInfoWidget()->SetCurrentVestDurabilityRate(CurDurability / DURABILITY_MAX);
		//UpdateDurabilityBar();
		Player->GetInvenSystem()->InitializeList();
	}

	return true;
}

float AC_Vest::GetDamageReduceFactor() const
{
	// 파괴된 조끼를 장착중일 때
	if (CurDurability <= 0.f) return 0.8f;
	
	
	return	(ItemLevel == EEquipableItemLevel::LV1) ? 0.7f :
			(ItemLevel == EEquipableItemLevel::LV2) ? 0.6f : 0.45f;
}


