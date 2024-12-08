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
	ItemDatas.ItemType = EItemTypes::BACKPACK;

	Level = 3;

	BackpackMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BackPackMesh"));
	RootComponent = BackpackMesh;

	static ConstructorHelpers::FObjectFinder<UTexture2D> ImageTexture(TEXT("/Game/Project_PUBG/Common/InventoryUI/Icons/Item/Equipment/Backpack/Item_Back_BlueBlocker.Item_Back_BlueBlocker"));
	ItemDatas.ItemIcon = ImageTexture.Object;

	ItemDatas.ItemName = TEXT("BackPack");

}

void AC_BackPack::BeginPlay()
{
	Super::BeginPlay();
}

void AC_BackPack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//void AC_BackPack::Interaction(AC_BasicCharacter* character)
//{
//	character->GetInvenComponent()->CheckMyBackPack(this);	
//}

void AC_BackPack::AttachToSocket(AC_BasicCharacter* character)
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

	SetOwnerCharacter(character);

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
	SetOwnerCharacter(nullptr);

	// 가방을 캐릭터의 발 아래로 이동시킴
	FVector DropLocation = character->GetActorLocation() + FVector(0.f, 0.f, -75.f);
	FRotator DropRotation = character->GetActorRotation() + FRotator(0.f, 0.f, -90.f);
	SetActorLocation(DropLocation);
	SetActorRotation(DropRotation);

	//SetActorLocation으로 꺼져버린 충돌을 다시 켜줌.
	SetActorEnableCollision(true);
	// 가방이 다시 보이게 설정
	BackpackMesh->SetSimulatePhysics(true);
	BackpackMesh->SetEnableGravity(true);
	BackpackMesh->SetVisibility(true);



}

bool AC_BackPack::Interaction(AC_BasicCharacter* Character)
{
	PickUpItem(Character);
	return true;//Character->GetInvenComponent()->CheckMyBackPack(this);
}

void AC_BackPack::PickUpItem(AC_BasicCharacter* Character)
{
	//캐릭터의 현재 용량과 바꾼 가방의 최대용량을 비교해서 바꾸기.
	UC_InvenComponent* InvenComp = Character->GetInvenComponent();
	AC_BackPack* curBackPack = nullptr;
	curBackPack = InvenComp->GetMyBackPack();

	float curVolume = InvenComp->GetCurVolume();
	float preMaxVolume = 70.f + InvenComp->CheckBackPackVolume(this->GetLevel());//갑빠가 더해주는 Volume 추가해야함.

	if (curVolume > preMaxVolume) return;

	if (curBackPack) 
		curBackPack->DetachmentItem();
	
	InvenComp->EquippedBackPack(this);

}

bool AC_BackPack::MoveToAround(AC_BasicCharacter* Character)
{
	UC_InvenComponent* InvenComp = Character->GetInvenComponent();
	AC_BackPack* curBackPack = nullptr;
	curBackPack = InvenComp->GetMyBackPack();

	float curVolume = InvenComp->GetCurVolume();
	float preMaxVolume = 70.f + InvenComp->CheckBackPackVolume(this->GetLevel());//갑빠가 더해주는 Volume 추가해야함.

	if (curVolume > preMaxVolume) return false;


	if (curBackPack)
	{
		//curBackPack->SetOwnerCharacter(nullptr);
		InvenComp->RemoveBackPack();
		curBackPack->DetachmentItem();
	}



	return true;
}
