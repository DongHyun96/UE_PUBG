// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/MeleeWeapon/C_MeleeWeapon.h"
#include "Item/Weapon/WeaponStrategy/I_WeaponButtonStrategy.h"
#include "Item/Weapon/WeaponStrategy/C_MeleeWeaponStrategy.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"

#include "Components/ShapeComponent.h"

#include "Kismet/GamePlayStatics.h"

#include "UObject/ConstructorHelpers.h"

#include "HUD/C_HUDWidget.h"
#include "HUD/C_AmmoWidget.h"

const FName AC_MeleeWeapon::HOLSTER_SOCKET_NAME = "Pan_Holster"; // ������ socket �̸�
const FName AC_MeleeWeapon::EQUIPPED_SOCKET_NAME = "Pan_Equip"; // ���Ⱑ �տ� ������ socket �̸�

AC_MeleeWeapon::AC_MeleeWeapon()
{
	WeaponButtonStrategy = CreateDefaultSubobject<AC_MeleeWeaponStrategy>("MeleeWeaponStrategy");

	AttackMontage.Priority = EMontagePriority::ATTACK;

	CurDrawMontage.Priority = EMontagePriority::DRAW_SHEATH_WEAPON;
	CurSheathMontage.Priority = EMontagePriority::DRAW_SHEATH_WEAPON;

	//ItemType ����.
	ItemDatas.ItemType = EItemTypes::MELEEWEAPON;
}

void AC_MeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

	AttackCollider = Cast<UShapeComponent>(GetComponentByClass(UShapeComponent::StaticClass()));
	AttackCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AttackCollider->OnComponentBeginOverlap.AddDynamic(this, &AC_MeleeWeapon::OnBodyColliderBeginOverlap);
}

void AC_MeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsValid(OwnerCharacter))
	{
		CurDrawMontage   = DrawMontages[OwnerCharacter->GetPoseState()];
		CurSheathMontage = SheathMontages[OwnerCharacter->GetPoseState()];
	}

}

bool AC_MeleeWeapon::AttachToHolster(USceneComponent* InParent)
{
	return AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		HOLSTER_SOCKET_NAME
	);
}

bool AC_MeleeWeapon::AttachToHand(USceneComponent* InParent)
{
	OwnerCharacter->SetHandState(EHandState::WEAPON_MELEE);

	return AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
		EQUIPPED_SOCKET_NAME
	);
}

void AC_MeleeWeapon::PickUpItem(AC_BasicCharacter* Character)
{
	if (!Character->GetInvenComponent()->CheckVolume(this))
	{
		//������ ������ ����, ����ĭ�� �ƹ��͵� ���ٸ� �����ؼ� ����.
		if (!Character->GetEquippedComponent()->GetWeapons().Find(EWeaponSlot::MELEE_WEAPON))
			Character->GetEquippedComponent()->SetSlotWeapon(EWeaponSlot::MELEE_WEAPON, this);
		return;  //����ĭ���� ���� �ִٸ� ����.
	}
	 
	SetOwnerCharacter(Character);
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

bool AC_MeleeWeapon::Interaction(AC_BasicCharacter* Character)
{
	AC_Weapon* curWeapaon = Character->GetEquippedComponent()->GetWeapons()[EWeaponSlot::MELEE_WEAPON];

	switch (ItemDatas.ItemPlace)
	{
	case EItemPlace::AROUND:
		if (curWeapaon) return LegacyMoveToInven(Character);
	case EItemPlace::INVEN:
		return LegacyMoveToSlot(Character);
	default:
		break;
	}
	return false;
}

bool AC_MeleeWeapon::LegacyMoveToInven(AC_BasicCharacter* Character)
{
	UC_InvenComponent* invenComp = Character->GetInvenComponent();

	if (invenComp->GetMaxVolume() < invenComp->GetCurVolume() + this->ItemDatas.ItemVolume) return false;

	invenComp->AddItemToMyList(this);
	//AttachToHolster(Character->GetMesh());
	//�κ����� ������ �ϸ� �̻��� ��ġ�� �ִ� ������ �̰ɷ� �켱 �ذ�.
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	return true; 
}

bool AC_MeleeWeapon::LegacyMoveToAround(AC_BasicCharacter* Character)
{
	if (!Character) return false;

	UC_InvenComponent* invenComp = Character->GetInvenComponent();
	//if (ItemDatas.ItemPlace == EItemPlace::SLOT)
	//TODO:���� ���� �ٸ� �Լ����� ���� ��� �� �� ����.
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	ItemDatas.ItemPlace = EItemPlace::AROUND;
	SetOwnerCharacter(nullptr);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	//if (ItemDatas.ItemPlace == EItemPlace::INVEN)
	invenComp->RemoveItemToMyList(this); //���⼭ �ӽ÷� ���ְ� �ִµ� ���� ������ ����.
		//invenComp->AddItemToAroundList(this);
	//�ٴ� ���� ĳ���� �޾ƿͼ� �ٴڿ� ������ �����ϱ�.
	SetActorLocation(GetGroundLocation(Character) + RootComponent->Bounds.BoxExtent.Z);
	return true;
}

bool AC_MeleeWeapon::LegacyMoveToSlot(AC_BasicCharacter* Character)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();
	UC_InvenComponent* invenComp = Character->GetInvenComponent();

	AC_Item* unEquipItem = nullptr; //������ ������

	AC_Item* inItem = nullptr; //������ ������

	AC_Weapon* curWeapaon = equipComp->GetWeapons()[EWeaponSlot::MELEE_WEAPON];

	if (IsValid(curWeapaon))
	{
		AC_Weapon* OutToSlotWeapon = nullptr;
		AC_Item* FoundItem = nullptr;

		int nextVolume = 0;

		if (this->ItemDatas.ItemPlace == EItemPlace::INVEN) 
			nextVolume = invenComp->GetCurVolume() - ItemDatas.ItemVolume + curWeapaon->GetItemDatas().ItemVolume;
		else
			nextVolume = invenComp->GetCurVolume() + curWeapaon->GetItemDatas().ItemVolume;

		if (nextVolume > invenComp->GetMaxVolume()) return false;
		else
		{
			unEquipItem = equipComp->SetSlotWeapon(EWeaponSlot::MELEE_WEAPON, this);
			invenComp->AddItemToMyList(unEquipItem);
			return true;
		}

	}
	else
	{
		equipComp->SetSlotWeapon(EWeaponSlot::MELEE_WEAPON, this);
		invenComp->RemoveItemToMyList(this);
		return true;
		//if (this->ItemDatas.ItemPlace == EItemPlace::INVEN)
	}
		
	//return false;
}

void AC_MeleeWeapon::SetAttackColliderEnabled(const bool& Enabled)
{
	if (Enabled) AttackCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	else		 AttackCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AC_MeleeWeapon::OnBodyColliderBeginOverlap
(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	// TODO : �ǰ�ü�� ������ �ֱ�

}

void AC_MeleeWeapon::OnOwnerCharacterPoseTransitionFin()
{
}

//void AC_MeleeWeapon::InitPriorityAnimMontages()
//{
//	AttackMontage.Priority = EMontagePriority::ATTACK;
//
//	/*FString leftMontagePath = FString::Printf
//	(
//		TEXT("/Game/Project_PUBG/DongHyun/Character/Animation/Turn_In_Place/Montages/%u%u_TurnLeft_Montage"),
//		handState,
//		poseState
//	);
//
//	ConstructorHelpers::FObjectFinder<UAnimMontage> TurnLeftMontage(*leftMontagePath);*/
//
//	//static ConstructorHelpers::FObjectFinder<UAnimMontage> attackAnimMontage
//	//(
//	//	TEXT("/Game/Project_PUBG/DongHyun/Character/Animation/WeaponMotions/MeleeMontages/MeleeAttack_Montage")
//	//);
//	//
//	//AttackMontage.AnimMontage = (attackAnimMontage.Succeeded()) ? attackAnimMontage.Object : nullptr;
//	//AttackMontage.Priority = EMontagePriority::ATTACK;
//
//	// Init Draw Montages
//
//	// Init Sheath Montages
//
//	//CurDrawMontage.Priority = EMontagePriority::DRAW_SHEATH_WEAPON;
//	//CurSheathMontage.Priority = EMontagePriority::DRAW_SHEATH_WEAPON;
//}
