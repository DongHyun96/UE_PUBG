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
	


	return false;
}

void AC_Vest::AttachToSocket(AC_BasicCharacter* InParent)
{
	FName SocketName{};
	switch (Level)
	{
	case 1:
		SocketName = "Vest_Lv1";
		break;
	case 2:
		SocketName = "Vest_Lv2";
		break;
	case 3:
		SocketName = "Vest_Lv3";
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
