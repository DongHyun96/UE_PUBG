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

	//ItemType 설정.
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

	//mobility가 Moveable이여야 장착 가능. 지금은 에디터 내에서 설정해 놓았음.
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
	//가방 해제.
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

	// 가방을 바닥에 스폰할 위치 계산.
	FVector DropLocation = character->GetActorLocation() + FVector(0.f, 0.f, -100.f); // 캐릭터 발 아래 위치 조정
	FRotator DropRotation = character->GetActorRotation();                            // 캐릭터의 현재 회전을 기준으로 설정

	// 가방 스폰.
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	this->GetWorld()->SpawnActor<AC_BackPack>(this->GetClass(), DropLocation, DropRotation, SpawnParams);
}
