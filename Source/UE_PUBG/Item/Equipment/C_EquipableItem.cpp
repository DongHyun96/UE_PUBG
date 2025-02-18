// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equipment/C_EquipableItem.h"
#include "Character/C_BasicCharacter.h"
#include "Utility/C_Util.h"

const float AC_EquipableItem::DURABILITY_MAX = 100.f;

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

	if (!Attached) UC_Util::Print("Not Attached", FColor::Cyan, 5.f);
}

bool AC_EquipableItem::TakeDamage(float DamageAmount)
{
	return false;
}

float AC_EquipableItem::GetDamageReduceFactor() const
{
	return 0.f;
}

bool AC_EquipableItem::MoveSlotToAround(AC_BasicCharacter* Character)
{
	return false;
}

bool AC_EquipableItem::MoveAroundToSlot(AC_BasicCharacter* Character)
{
	return false;
}


