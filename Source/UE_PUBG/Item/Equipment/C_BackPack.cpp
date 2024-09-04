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

	//mobility�� Moveable�̿��� ���� ����. ������ ������ ������ ������ ������.
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
	//���� ����.

	if (!character->GetInvenComponent()->GetMyBackPack()) return;

	//USkeletalMeshComponent* backpackMesh = this->FindComponentByClass<USkeletalMeshComponent>();
	BackpackMesh = FindComponentByClass<USkeletalMeshComponent>();
	if (BackpackMesh)
	{
		// ĳ���Ϳ��� ������ �и�
		BackpackMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

		// ������ ������ �ʰ� ����
		BackpackMesh->SetVisibility(false);
		//�浹�� ���⼭ ���ְ� SetLocation ���Ŀ� �ٽ� ���ָ� OverlappedBegin�̺�Ʈ�� �۵���.
		SetActorEnableCollision(false);
	}

	// ������ ĳ������ �� �Ʒ��� �̵���Ŵ
	FVector DropLocation = character->GetActorLocation() + FVector(0.f, 0.f, -75.f);
	FRotator DropRotation = character->GetActorRotation() + FRotator(0.f, 0.f, -90.f);
	SetActorLocation(DropLocation);
	SetActorRotation(DropRotation);

	//SetActorLocation���� �������� �浹�� �ٽ� ����.
	this->SetActorEnableCollision(true);
	// ������ �ٽ� ���̰� ����
	BackpackMesh->SetSimulatePhysics(true);
	BackpackMesh->SetEnableGravity(true);
	BackpackMesh->SetVisibility(true);


}
