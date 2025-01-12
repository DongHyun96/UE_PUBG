// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Equipment/C_BackPack.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Components/SphereComponent.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Utility/C_Util.h"
#include "Character/Component/C_InvenComponent.h"


AC_BackPack::AC_BackPack()
{
	PrimaryActorTick.bCanEverTick = true;

	//ItemType ����.
	ItemDatas.ItemType = EItemTypes::BACKPACK;

	Level = 3;

	//BackpackMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BackPackMesh"));
	//RootComponent = BackpackMesh;

	static ConstructorHelpers::FObjectFinder<UTexture2D> ImageTexture(TEXT("/Game/Project_PUBG/Common/InventoryUI/Icons/Item/Equipment/Backpack/Item_Back_BlueBlocker.Item_Back_BlueBlocker"));
	ItemDatas.ItemBarIcon = ImageTexture.Object;

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

void AC_BackPack::AttachToSocket(AC_BasicCharacter* InParent)
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

void AC_BackPack::DetachToSocket(AC_BasicCharacter* character)
{
	//���� ����.

	if (!character->GetInvenComponent()->GetMyBackPack()) return;
	DetachItem();
	////USkeletalMeshComponent* backpackMesh = this->FindComponentByClass<USkeletalMeshComponent>();
	//BackpackMesh = FindComponentByClass<UStaticMeshComponent>();
	//if (BackpackMesh)
	//{
	//	// ĳ���Ϳ��� ������ �и�
	//	BackpackMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	//
	//	// ������ ������ �ʰ� ����
	//	BackpackMesh->SetVisibility(false);
	//	//�浹�� ���⼭ ���ְ� SetLocation ���Ŀ� �ٽ� ���ָ� OverlappedBegin�̺�Ʈ�� �۵���.
	//	SetActorEnableCollision(false);
	//}
	//SetOwnerCharacter(nullptr);
	//
	//// ������ ĳ������ �� �Ʒ��� �̵���Ŵ
	//FVector DropLocation = character->GetActorLocation() + FVector(0.f, 0.f, -75.f);
	//FRotator DropRotation = character->GetActorRotation() + FRotator(0.f, 0.f, -90.f);
	//SetActorLocation(DropLocation);
	//SetActorRotation(DropRotation);
	//
	////SetActorLocation���� �������� �浹�� �ٽ� ����.
	//SetActorEnableCollision(true);
	//// ������ �ٽ� ���̰� ����
	//BackpackMesh->SetSimulatePhysics(true);
	//BackpackMesh->SetEnableGravity(true);
	//BackpackMesh->SetVisibility(true);



}

bool AC_BackPack::Interaction(AC_BasicCharacter* Character)
{
	//PickUpItem(Character);
	switch (GetItemDatas().ItemPlace)
	{
	case EItemPlace::AROUND:
		return LegacyMoveToSlot(Character);
	case EItemPlace::SLOT:
		return LegacyMoveToAround(Character);
	default:
		return false;
	}
	//return false;//Character->GetInvenComponent()->CheckMyBackPack(this);
}

void AC_BackPack::PickUpItem(AC_BasicCharacter* Character)
{
	//ĳ������ ���� �뷮�� �ٲ� ������ �ִ�뷮�� ���ؼ� �ٲٱ�.
	UC_InvenComponent* InvenComp = Character->GetInvenComponent();
	AC_BackPack* curBackPack = nullptr;
	curBackPack = InvenComp->GetMyBackPack();

	float curVolume = InvenComp->GetCurVolume();
	float preMaxVolume = 70.f + InvenComp->CheckBackPackVolume(this->GetLevel());//������ �����ִ� Volume �߰��ؾ���.

	if (curVolume > preMaxVolume) return;

	if (curBackPack) 
		curBackPack->DetachItem();
	
	InvenComp->EquippedBackPack(this);

	this->SetItemPlace(EItemPlace::SLOT);

}

bool AC_BackPack::LegacyMoveToAround(AC_BasicCharacter* Character)
{
	UC_InvenComponent* InvenComp = Character->GetInvenComponent();
	AC_BackPack* curBackPack = nullptr;
	curBackPack = Cast<AC_BackPack>(InvenComp->GetEquipmentItems()[EEquipSlot::BACKPACK]);

	float curVolume = InvenComp->GetCurVolume();
	float preMaxVolume = 70.f + InvenComp->CheckBackPackVolume(this->GetLevel());//TODO : ������ �����ִ� Volume �߰��ؾ���.

	if (curVolume > preMaxVolume) return false;

	if (curBackPack)
	{
		//curBackPack->SetOwnerCharacter(nullptr);
		InvenComp->SetSlotEquipment(EEquipSlot::BACKPACK, curBackPack);
		//InvenComp->RemoveBackPack();
		//curBackPack->DetachItem();
	}
	 
	InvenComp->CheckBackPackOnCharacter();

	this->SetItemPlace(EItemPlace::AROUND);

	return true;
}

bool AC_BackPack::LegacyMoveToSlot(AC_BasicCharacter* Character)
{
	//TODO : PickUpItem �������� �켱 ������ ��. �ٽ� �����ϱ�.
	//ĳ������ ���� �뷮�� �ٲ� ������ �ִ�뷮�� ���ؼ� �ٲٱ�.
	UC_InvenComponent* InvenComp = Character->GetInvenComponent();
	UC_EquippedComponent* EquipComp = Character->GetEquippedComponent();

	AC_BackPack* curBackPack = nullptr;
	curBackPack = Cast<AC_BackPack>(InvenComp->GetEquipmentItems()[EEquipSlot::BACKPACK]);

	float curVolume = InvenComp->GetCurVolume();
	float preMaxVolume = 70.f + InvenComp->CheckBackPackVolume(this->GetLevel());//������ �����ִ� Volume �߰��ؾ���.

	if (curVolume > preMaxVolume) return false;

	if (curBackPack)
		curBackPack->LegacyMoveToAround(Character);

	//InvenComp->EquippedBackPack(this);

	InvenComp->SetSlotEquipment(EEquipSlot::BACKPACK, this);

	InvenComp->CheckBackPackOnCharacter();
			
	this->SetItemPlace(EItemPlace::SLOT);
	
	return true;
}

bool AC_BackPack::MoveSlotToAround(AC_BasicCharacter* Character)
{
	UC_InvenComponent* InvenComp = Character->GetInvenComponent();
	AC_BackPack* curBackPack = nullptr;
	curBackPack = Cast<AC_BackPack>(InvenComp->GetEquipmentItems()[EEquipSlot::BACKPACK]);

	float curVolume = InvenComp->GetCurVolume();
	float preMaxVolume = 70.f + InvenComp->CheckBackPackVolume(this->GetLevel());//TODO : ������ �����ִ� Volume �߰��ؾ���.

	if (curVolume > preMaxVolume) return false;

	if (curBackPack)
	{
		//curBackPack->SetOwnerCharacter(nullptr);
		InvenComp->SetSlotEquipment(EEquipSlot::BACKPACK, curBackPack);
		//InvenComp->RemoveBackPack();
		//curBackPack->DetachItem();
	}

	InvenComp->CheckBackPackOnCharacter();

	this->SetItemPlace(EItemPlace::AROUND);

	return true;
}

bool AC_BackPack::MoveAroundToSlot(AC_BasicCharacter* Character)
{
	//TODO : PickUpItem �������� �켱 ������ ��. �ٽ� �����ϱ�.
//ĳ������ ���� �뷮�� �ٲ� ������ �ִ�뷮�� ���ؼ� �ٲٱ�.
	UC_InvenComponent* InvenComp = Character->GetInvenComponent();
	UC_EquippedComponent* EquipComp = Character->GetEquippedComponent();

	AC_BackPack* curBackPack = nullptr;
	curBackPack = Cast<AC_BackPack>(InvenComp->GetEquipmentItems()[EEquipSlot::BACKPACK]);

	float curVolume = InvenComp->GetCurVolume();
	float preMaxVolume = 70.f + InvenComp->CheckBackPackVolume(this->GetLevel());//������ �����ִ� Volume �߰��ؾ���.

	if (curVolume > preMaxVolume) return false;

	if (curBackPack)
		curBackPack->LegacyMoveToAround(Character);

	//InvenComp->EquippedBackPack(this);

	InvenComp->SetSlotEquipment(EEquipSlot::BACKPACK, this);

	InvenComp->CheckBackPackOnCharacter();

	//this->SetItemPlace(EItemPlace::SLOT);

	return true;
}
