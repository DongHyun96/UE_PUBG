// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equipment/C_EquipableItem.h"
#include "Character/C_BasicCharacter.h"
#include "Utility/C_Util.h"

AC_EquipableItem::AC_EquipableItem()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_EquipableItem::BeginPlay()
{
	Super::BeginPlay();
}


void AC_EquipableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AC_EquipableItem::Interaction(AC_BasicCharacter* Character)
{
	return false;
}

void AC_EquipableItem::AttachToSocket(AC_BasicCharacter* InParent)
{
	FName SocketName{};
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
