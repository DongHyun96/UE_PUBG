// Fill out your copyright notice in the Description page of Project Settings.



#include "Item/Attachment/C_AttachableItem.h"
#include "Item/Weapon/Gun/C_Gun.h"
//#include "Item/Weapon/WeaponStrategy/C_GunStrategy.h"

//#include "Blueprint/UserWidget.h"
//#include "Blueprint/WidgetBlueprintLibrary.h"

//#include "UMG.h"
//#include "Components/CanvasPanelSlot.h"
//#include "Components/Image.h"
//#include "Components/Widget.h"
//#include "Components/PanelWidget.h"
//#include "Components/NamedSlotInterface.h"
//#include "Components/SkeletalMeshComponent.h"

#include "Components/CapsuleComponent.h"
//#include "Components/ShapeComponent.h"
//#include "Components/SceneComponent.h"

//#include "Animation/AnimInstance.h"
//#include "Animation/AnimMontage.h"
//#include "Camera/CameraComponent.h"
//#include "Kismet/GameplayStatics.h"

#include "Character/C_BasicCharacter.h"
//#include "Character/C_Player.h"

//#include "Character/Component/C_CrosshairWidgetComponent.h"
#include "Character/Component/C_AttachableItemMeshComponent.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"

//#include "GameFramework/Actor.h"
//#include "GameFramework/CharacterMovementComponent.h"
//#include "GameFramework/SpringArmComponent.h"

#include "Utility/C_Util.h"

#include "Item/Weapon/Gun/C_Bullet.h"
AC_AttachableItem::AC_AttachableItem()
{

}

void AC_AttachableItem::BeginPlay()
{
	Super::BeginPlay();
	//ItemDatas.ItemName = "Test Item Name";
	AttachableItemMesh = FindComponentByClass<USkeletalMeshComponent>();
	CapsuleCollider = FindComponentByClass<UCapsuleComponent>();
	if (IsValid(AttachableItemMesh))
	{
		AttachableItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//UC_Util::Print("Found mesh!!!!!!", FColor::Blue);
	}
}

void AC_AttachableItem::Tick(float DeltaTime)
{	
	Super::Tick(DeltaTime);
}

bool AC_AttachableItem::Interaction(AC_BasicCharacter* Character)
{
	return MoveToSlot(Character);
}

bool AC_AttachableItem::MoveAroundToInven(AC_BasicCharacter* Character)
{
	UC_InvenComponent* invenComp = Character->GetInvenComponent();

	if (!invenComp->CheckVolume(this)) return false;

	invenComp->RemoveItemToAroundList(this);
	
	invenComp->AddItemToMyList(this);

	SetActorHiddenInGame(true);

	SetActorEnableCollision(false);

	return true;
}

bool AC_AttachableItem::MoveAroundToSlot(AC_BasicCharacter* Character)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();
	
	AC_Gun* curWeapon = nullptr;
	
	if (curWeapon = Cast<AC_Gun>(equipComp->GetWeapons()[EWeaponSlot::MAIN_GUN]))
	{
		if (this->AttachItemToWeaponAndMove(curWeapon, Character))
			return true;
	}

	if (curWeapon = Cast<AC_Gun>(equipComp->GetWeapons()[EWeaponSlot::SUB_GUN]))
	{
		if (this->AttachItemToWeaponAndMove(curWeapon, Character))
			return true;
	}

	return MoveToInven(Character);
}

bool AC_AttachableItem::MoveInvenToAround(AC_BasicCharacter* Character)
{
	UC_InvenComponent* invenComp = Character->GetInvenComponent();

	//인벤에 해당 아이템이 있는지 안전체크 해야 할까?

	invenComp->RemoveItemToMyList(this);



	//SetOwnerCharacter(nullptr);

	DropItem(Character);

	return true;
}

bool AC_AttachableItem::MoveInvenToSlot(AC_BasicCharacter* Character)
{
	UC_InvenComponent* invenComp = Character->GetInvenComponent();
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();

	AC_Gun* curWeapon = nullptr;

	if (curWeapon = Cast<AC_Gun>(equipComp->GetWeapons()[EWeaponSlot::MAIN_GUN]))
	{
		return this->AttachItemToWeaponAndMoveInven(curWeapon, invenComp);
	}

	if (curWeapon = Cast<AC_Gun>(equipComp->GetWeapons()[EWeaponSlot::SUB_GUN]))
	{
		return this->AttachItemToWeaponAndMoveInven(curWeapon, invenComp);
	}
	return false;
}

bool AC_AttachableItem::MoveSlotToAround(AC_BasicCharacter* Character)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();

	GetOwnerGun()->SetAttachableItemSlot(this->Name, nullptr);

	SetOwnerGun(nullptr);

	DropItem(Character);
	
	return false;
}

bool AC_AttachableItem::MoveSlotToInven(AC_BasicCharacter* Character)
{
	//if (!GetOwnerGun()) return false;

	UC_InvenComponent* invenComp = Character->GetInvenComponent();

	if (!invenComp->CheckVolume(this)) return false; //인벤에 공간이 부족하면 return false;

	GetOwnerGun()->SetAttachableItemSlot(this->Name, nullptr);

	SetOwnerGun(nullptr);

	invenComp->AddItemToMyList(this);

	return true;
}

bool AC_AttachableItem::MoveSlotToSlot(AC_BasicCharacter* Character)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();

	AC_Gun* curGun = GetOwnerGun();

	if (!curGun) return false;

	if (curGun->GetWeaponSlot() == EWeaponSlot::MAIN_GUN)
	{
		AC_Gun* OtherGun = Cast<AC_Gun>(equipComp->GetWeapons()[EWeaponSlot::SUB_GUN]);
		return this->SwapAttachableItem(curGun, OtherGun);
	}
	else if (curGun->GetWeaponSlot() == EWeaponSlot::SUB_GUN)
	{
		AC_Gun* OtherGun = Cast<AC_Gun>(equipComp->GetWeapons()[EWeaponSlot::MAIN_GUN]);
		return this->SwapAttachableItem(curGun, OtherGun);
	}

	return false;
}

void AC_AttachableItem::SetIsAttached(bool InIsAttachedToWeapon)
{
	if (InIsAttachedToWeapon)
	{
		if (IsValid(CapsuleCollider))
			CapsuleCollider->SetCollisionProfileName(TEXT("NoCollision"));
	}
}

void AC_AttachableItem::SetMeshVisibility(bool InIsVisible)
{
	AttachableItemMesh->SetHiddenInGame(!InIsVisible);
	//CapsuleCollider->SetVisibility(InIsVisible);
	//AttachableItemMesh->SetVisibility(InIsVisible);
	//UC_Util::Print("jasyhdfbuiayersyhgbfbaueysdhyfbgasodudyfgbasdudlkyfgbaeruoyfgbbaweuoyfgbeqaouryfgvbwaeuikyrf");
}

bool AC_AttachableItem::AttachItemToWeaponAndMove(AC_Gun* Weapon, AC_BasicCharacter* Character)
{
	if (!Weapon->GetAttachableParts().Contains(this->Name)) return false; // 부착 가능 여부 확인

	UC_InvenComponent* invenComp = Character->GetInvenComponent();
	AC_AttachableItem* ChangedItem = Weapon->GetAttachableItem()[Name];

	if (ChangedItem) //return false; // ChangedItem이 nullptr 라면 return false;
	{
		// 부착된 아이템을 인벤토리 또는 주변 리스트로 이동
		if (!ChangedItem->MoveToInven(Character)) //왜 중간에 return false를 넘어가서 마지막 return true까지 갔는데도 결과가 false지?-> MoveTo~에서 return을 제대로 안해줬음.
			ChangedItem->MoveToAround(Character);
	}
	Weapon->SetAttachableItemSlot(this->Name, this);

	SetActorHiddenInGame(true);

	SetActorEnableCollision(false);

	SetOwnerGun(Weapon);
	
	invenComp->RemoveItemToAroundList(this);

	// TODO: AroundList에서 사라지는지 확인하기
	return true;
}

bool AC_AttachableItem::AttachItemToWeaponAndMoveInven(AC_Gun* Weapon, UC_InvenComponent* invenComp)
{
	if (!Weapon->GetAttachableParts().Contains(this->Name)) return false; // 부착 가능 여부 확인
	
	AC_AttachableItem* ChangedItem = Weapon->GetAttachableItem()[Name];
	
	invenComp->RemoveItemToMyList(this);  // 인벤토리에서 아이템 제거
	
	if (ChangedItem)
	{
		if (!ChangedItem->MoveToInven(OwnerCharacter))
			ChangedItem->MoveToAround(OwnerCharacter);
	}
	Weapon->SetAttachableItemSlot(this->Name, this);  // 부착물 설정

	SetOwnerGun(Weapon);

	invenComp->RemoveItemToMyList(this);


	// TODO: AroundList에서 사라지는지 확인하기

	return true;
}

bool AC_AttachableItem::SwapAttachableItem(AC_Gun* CurrentGun, AC_Gun* OtherGun)
{
	if (!OtherGun || !OtherGun->GetAttachableParts().Contains(Name))
	{
		return false;
	}

	AC_AttachableItem* SwappedItem = OtherGun->SetAttachableItemSlot(Name, this);
	CurrentGun->SetAttachableItemSlot(Name, SwappedItem);

	return true;
}