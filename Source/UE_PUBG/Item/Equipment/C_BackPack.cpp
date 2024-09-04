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

	static ConstructorHelpers::FObjectFinder<UTexture2D> ImageTexture(TEXT("/Game/Project_PUBG/Common/InventoryUI/Icons/Item/Equipment/Backpack/Item_Back_BlueBlocker.Item_Back_BlueBlocker"));
	ItemIcon = ImageTexture.Object;

	ItemName = TEXT("BackPack");

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
	character->GetInvenComponent()->CheckMyBackPack(this);	
}

void AC_BackPack::AttachToSocket(AC_BasicCharacter* character)
{
	FName SocketName = "BackPackSocket";

	//mobility가 Moveable이여야 장착 가능. 지금은 에디터 내에서 설정해 놓았음.
	//BackpackMesh->SetMobility(EComponentMobility::Movable);

	bool Attached = AttachToComponent
	(
		character->GetMesh(),
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

	if (!character->GetInvenComponent()->GetMyBackPack()) return;

	//USkeletalMeshComponent* backpackMesh = this->FindComponentByClass<USkeletalMeshComponent>();
	BackpackMesh = FindComponentByClass<USkeletalMeshComponent>();
	if (BackpackMesh)
	{
		// 캐릭터에서 가방을 분리
		BackpackMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

		// 가방이 보이지 않게 설정
		BackpackMesh->SetVisibility(false);
		//충돌을 여기서 꺼주고 SetLocation 이후에 다시 켜주면 OverlappedBegin이벤트가 작동함.
		SetActorEnableCollision(false);
	}

	// 가방을 캐릭터의 발 아래로 이동시킴
	FVector DropLocation = character->GetActorLocation() + FVector(0.f, 0.f, -75.f);
	FRotator DropRotation = character->GetActorRotation() + FRotator(0.f, 0.f, -90.f);
	SetActorLocation(DropLocation);
	SetActorRotation(DropRotation);

	//SetActorLocation으로 꺼져버린 충돌을 다시 켜줌.
	this->SetActorEnableCollision(true);
	// 가방이 다시 보이게 설정
	BackpackMesh->SetSimulatePhysics(true);
	BackpackMesh->SetEnableGravity(true);
	BackpackMesh->SetVisibility(true);


}
