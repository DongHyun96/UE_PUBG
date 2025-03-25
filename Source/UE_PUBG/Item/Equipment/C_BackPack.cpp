// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Equipment/C_BackPack.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_InvenSystem.h"
#include "InvenUI/C_InventoryUIWidget.h"
#include "Components/SphereComponent.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Utility/C_Util.h"
#include "HUD/C_ArmorInfoWidget.h"
#include "HUD/C_HUDWidget.h"


AC_BackPack::AC_BackPack()
{
	PrimaryActorTick.bCanEverTick = true;

	//ItemType 설정.
	//ItemDataRef->ItemType = EItemTypes::BACKPACK;

	Level = 3;

	//BackpackMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BackPackMesh"));
	//RootComponent = BackpackMesh;

	//static ConstructorHelpers::FObjectFinder<UTexture2D> ImageTexture(TEXT("/Game/Project_PUBG/Common/InventoryUI/Icons/Item/Equipment/Backpack/Item_Back_BlueBlocker.Item_Back_BlueBlocker"));
	//ItemDataRef.ItemBarIcon = ImageTexture.Object;
	//
	//ItemDataRef.ItemName = TEXT("BackPack");

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

	// Player일 경우, HUD Armor info widget 업데이트
	if (AC_Player* Player = Cast<AC_Player>(InParent))
	{
		float CurMaxVolume	= Player->GetInvenComponent()->GetMaxVolume();
		float CurVolume		= Player->GetInvenComponent()->GetCurVolume();
		Player->GetHUDWidget()->GetArmorInfoWidget()->SetBackPackInfo(static_cast<uint8>(ItemLevel) + 1, CurVolume / CurMaxVolume);
	}
	
	if (!Attached) UC_Util::Print("Not Attached", FColor::Cyan, 5.f);
}

void AC_BackPack::DetachToSocket(AC_BasicCharacter* character)
{
	//가방 해제.

	if (!character->GetInvenComponent()->GetMyBackPack()) return;
	DetachItem();
	////USkeletalMeshComponent* backpackMesh = this->FindComponentByClass<USkeletalMeshComponent>();
	//BackpackMesh = FindComponentByClass<UStaticMeshComponent>();
	//if (BackpackMesh)
	//{
	//	// 캐릭터에서 가방을 분리
	//	BackpackMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	//
	//	// 가방이 보이지 않게 설정
	//	BackpackMesh->SetVisibility(false);
	//	//충돌을 여기서 꺼주고 SetLocation 이후에 다시 켜주면 OverlappedBegin이벤트가 작동함.
	//	SetActorEnableCollision(false);
	//}
	//SetOwnerCharacter(nullptr);
	//
	//// 가방을 캐릭터의 발 아래로 이동시킴
	//FVector DropLocation = character->GetActorLocation() + FVector(0.f, 0.f, -75.f);
	//FRotator DropRotation = character->GetActorRotation() + FRotator(0.f, 0.f, -90.f);
	//SetActorLocation(DropLocation);
	//SetActorRotation(DropRotation);
	//
	////SetActorLocation으로 꺼져버린 충돌을 다시 켜줌.
	//SetActorEnableCollision(true);
	//// 가방이 다시 보이게 설정
	//BackpackMesh->SetSimulatePhysics(true);
	//BackpackMesh->SetEnableGravity(true);
	//BackpackMesh->SetVisibility(true);



}

bool AC_BackPack::Interaction(AC_BasicCharacter* Character)
{
	//PickUpItem(Character);
	switch (ItemPlace)
	{
	case EItemPlace::AROUND:
		//return LegacyMoveToSlot(Character);
		return MoveToSlot(Character, GetItemCurStack());
	case EItemPlace::SLOT:
		//return LegacyMoveToAround(Character);
		return MoveToAround(Character, GetItemCurStack());
	default:
		return false;
	}
	//return false;//Character->GetInvenComponent()->CheckMyBackPack(this);
}

//void AC_BackPack::PickUpItem(AC_BasicCharacter* Character)
//{
//	//캐릭터의 현재 용량과 바꾼 가방의 최대용량을 비교해서 바꾸기.
//	UC_InvenComponent* InvenComp = Character->GetInvenComponent();
//	AC_BackPack* curBackPack = nullptr;
//	curBackPack = InvenComp->GetMyBackPack();
//
//	float curVolume = InvenComp->GetCurVolume();
//	float preMaxVolume = 70.f + InvenComp->CheckBackPackVolume(this->GetLevel());//갑빠가 더해주는 Volume 추가해야함.
//
//	if (curVolume > preMaxVolume) return;
//
//	if (curBackPack) 
//		curBackPack->DetachItem();
//	
//	InvenComp->EquippedBackPack(this);
//
//	this->SetItemPlace(EItemPlace::SLOT);
//
//}

bool AC_BackPack::LegacyMoveToAround(AC_BasicCharacter* Character)
{
	
	UC_InvenComponent* InvenComp = Character->GetInvenComponent();
	AC_BackPack* curBackPack = nullptr;
	curBackPack = Cast<AC_BackPack>(InvenComp->GetEquipmentItems()[EEquipSlot::BACKPACK]);

	float curVolume = InvenComp->GetCurVolume();
	float preMaxVolume = 70.f + InvenComp->CheckBackPackVolume(this->GetLevel());//TODO : 갑빠가 더해주는 Volume 추가해야함.

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

	// Player일 경우, Armor info widget 업데이트
	if (AC_Player* Player = Cast<AC_Player>(Character))
		Player->GetHUDWidget()->GetArmorInfoWidget()->SetBackPackInfo(0);

	return true;
}

bool AC_BackPack::LegacyMoveToSlot(AC_BasicCharacter* Character)
{
	
	//TODO : PickUpItem 내용으로 우선 구현한 것. 다시 구현하기.
	//캐릭터의 현재 용량과 바꾼 가방의 최대용량을 비교해서 바꾸기.
	UC_InvenComponent* InvenComp = Character->GetInvenComponent();
	UC_EquippedComponent* EquipComp = Character->GetEquippedComponent();

	AC_BackPack* curBackPack = nullptr;
	curBackPack = Cast<AC_BackPack>(InvenComp->GetEquipmentItems()[EEquipSlot::BACKPACK]);

	float curVolume = InvenComp->GetCurVolume();
	float preMaxVolume = 70.f + InvenComp->CheckBackPackVolume(this->GetLevel());//갑빠가 더해주는 Volume 추가해야함.

	if (curVolume > preMaxVolume) return false;

	if (curBackPack)
		curBackPack->LegacyMoveToAround(Character);

	//InvenComp->EquippedBackPack(this);

	InvenComp->SetSlotEquipment(EEquipSlot::BACKPACK, this);

	InvenComp->CheckBackPackOnCharacter();
			
	this->SetItemPlace(EItemPlace::SLOT);
	
	return true;
}

bool AC_BackPack::MoveSlotToAround(AC_BasicCharacter* Character,int32 InStack)
{
	
	UC_InvenComponent* InvenComp = Character->GetInvenComponent();
	AC_BackPack* curBackPack = nullptr;
	curBackPack = Cast<AC_BackPack>(InvenComp->GetEquipmentItems()[EEquipSlot::BACKPACK]);

	float curVolume = InvenComp->GetCurVolume();
	float preMaxVolume = 70.f;// +InvenComp->CheckBackPackVolume(this->GetLevel());//TODO : 갑빠가 더해주는 Volume 추가해야함.

	if (curVolume > preMaxVolume) return false;

	if (curBackPack)
	{
		//curBackPack->SetOwnerCharacter(nullptr);
		InvenComp->SetSlotEquipment(EEquipSlot::BACKPACK, nullptr);
		//InvenComp->RemoveBackPack();
		//curBackPack->DetachItem();
	}

	InvenComp->CheckBackPackOnCharacter();

	this->SetItemPlace(EItemPlace::AROUND);

	// Player일 경우, Armor info widget 업데이트
	if (AC_Player* Player = Cast<AC_Player>(Character))
		Player->GetHUDWidget()->GetArmorInfoWidget()->SetBackPackInfo(0);

	return true;
}

bool AC_BackPack::MoveAroundToSlot(AC_BasicCharacter* Character, int32 InStack)
{

	
	//TODO : PickUpItem 내용으로 우선 구현한 것. 다시 구현하기.
	//캐릭터의 현재 용량과 바꾼 가방의 최대용량을 비교해서 바꾸기.
	UC_InvenComponent* InvenComp = Character->GetInvenComponent();
	UC_EquippedComponent* EquipComp = Character->GetEquippedComponent();

	AC_BackPack* curBackPack = nullptr;
	curBackPack = Cast<AC_BackPack>(InvenComp->GetEquipmentItems()[EEquipSlot::BACKPACK]);

	float curVolume = InvenComp->GetCurVolume();
	float preMaxVolume = 70.f + InvenComp->CheckBackPackVolume(this->GetLevel()) + InvenComp->GetVestVolume();//갑빠가 더해주는 Volume 추가해야함.

	if (curVolume >= preMaxVolume) return false;

	if (curBackPack)
		curBackPack->MoveToAround(Character, InStack);

	//InvenComp->EquippedBackPack(this);

	InvenComp->SetSlotEquipment(EEquipSlot::BACKPACK, this);

	InvenComp->CheckBackPackOnCharacter();

	//this->SetItemPlace(EItemPlace::SLOT);

	return true;
}


