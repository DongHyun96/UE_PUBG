// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Equipment/C_BackPack.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Components/SphereComponent.h"
#include "Utility/C_Util.h"
#include "Character/Component/C_InvenComponent.h"


AC_BackPack::AC_BackPack()
{
	PrimaryActorTick.bCanEverTick = true;

	//ItemType ����.
	MyItemType = EItemTypes::BACKPACK;

	Level = 3;

	BackpackMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BackPackMesh"));
	RootComponent = BackpackMesh;

}

void AC_BackPack::BeginPlay()
{
	Super::BeginPlay();
}

void AC_BackPack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_BackPack::Interaction(AC_BasicCharacter* character)
{
	character->GetInvenComponent()->ChackMyBackPack(this);	

}

void AC_BackPack::AttachToSocket(USceneComponent* InParent)
{
	FName SocketName = "BackPackSocket";

	//mobility�� Moveable�̿��� ���� ����. ������ ������ ������ ������ ������.
	//BackpackMesh->SetMobility(EComponentMobility::Movable);

	bool Attached = AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		SocketName
	);

	//BackpackMesh->SetVisibility(true);

	//DrawDebugSphere(GetWorld(), InParent->GetSocketLocation(SocketName), 10.f, 12, FColor::Red, false, 10.f);

	if (!Attached) UC_Util::Print("Not Attached", FColor::Cyan, 5.f);
}

void AC_BackPack::DetachToSocket(AC_BasicCharacter* character)
{
	//���� ����.
	if (character->GetInvenComponent()->GetMyBackPack())
	{
		UStaticMeshComponent* backpackMesh = character->GetInvenComponent()->GetMyBackPack()->FindComponentByClass<UStaticMeshComponent>();
		if (backpackMesh)
		{
			backpackMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		}
	}
	else
	{
		return;
	}

	// ������ �ٴڿ� ������ ��ġ ���.
	FVector DropLocation = character->GetActorLocation() + FVector(0.f, 0.f, -100.f); // ĳ���� �� �Ʒ� ��ġ ����
	FRotator DropRotation = character->GetActorRotation();                            // ĳ������ ���� ȸ���� �������� ����

	// ���� ����.
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	this->GetWorld()->SpawnActor<AC_BackPack>(this->GetClass(), DropLocation, DropRotation, SpawnParams);
}
