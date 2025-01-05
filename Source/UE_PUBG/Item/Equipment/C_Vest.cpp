// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equipment/C_Vest.h"
#include "Character/C_BasicCharacter.h"
#include "Utility/C_Util.h"

AC_Vest::AC_Vest()
{
	PrimaryActorTick.bCanEverTick = true;

}

bool AC_Vest::Interaction(AC_BasicCharacter* Character)
{
	Character->GetInvenComponent()->SetSlotEquipment(EEquipSlot::VEST, this);

	return true;
}

bool AC_Vest::LegacyMoveToAround(AC_BasicCharacter* Character)
{
	Character->GetInvenComponent()->SetSlotEquipment(EEquipSlot::VEST, nullptr);
	OwnerCharacter = nullptr;

	return false;
}

bool AC_Vest::LegacyMoveToSlot(AC_BasicCharacter* Character)
{
	//TODO : �����Ǿ� �ִ� Vest ó�����ֱ� �� ����� �����ϱ�, SetSlotEquipment�Լ� �����ϱ�.
	Character->GetInvenComponent()->SetSlotEquipment(EEquipSlot::VEST, this);
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

	//mobility�� Moveable�̿��� ���� ����. ������ ������ ������ ������ ������.
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

	if (!Attached) UC_Util::Print("Not Attached", FColor::Cyan, 5.f);
}
