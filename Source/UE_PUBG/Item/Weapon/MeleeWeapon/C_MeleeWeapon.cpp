// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/MeleeWeapon/C_MeleeWeapon.h"

#include "Item/Weapon/WeaponStrategy/C_MeleeWeaponStrategy.h"

#include "Character/C_BasicCharacter.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"

#include "Components/ShapeComponent.h"

#include "UObject/ConstructorHelpers.h"

#include "Kismet/KismetMathLibrary.h"
#include "Utility/C_Util.h"

const FName AC_MeleeWeapon::HOLSTER_SOCKET_NAME = "Pan_Holster"; // 무기집 socket 이름
const FName AC_MeleeWeapon::EQUIPPED_SOCKET_NAME = "Pan_Equip"; // 무기가 손에 부착될 socket 이름

AC_MeleeWeapon::AC_MeleeWeapon()
{
	WeaponButtonStrategy = CreateDefaultSubobject<AC_MeleeWeaponStrategy>("MeleeWeaponStrategy");

	AttackMontage.Priority = EMontagePriority::ATTACK;

	CurDrawMontage.Priority = EMontagePriority::DRAW_SHEATH_WEAPON;
	CurSheathMontage.Priority = EMontagePriority::DRAW_SHEATH_WEAPON;

	//ItemType 설정.
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
		
		if (AttackCollider->GetCollisionEnabled() == ECollisionEnabled::QueryAndPhysics) UC_Util::Print("CollisionEnabled");
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
		//공간이 없으면 실행, 근접칸에 아무것도 없다면 실행해서 장착.
		if (!Character->GetEquippedComponent()->GetWeapons().Find(EWeaponSlot::MELEE_WEAPON))
			Character->GetEquippedComponent()->SetSlotWeapon(EWeaponSlot::MELEE_WEAPON, this);
		return;  //근접칸에도 뭔가 있다면 리턴.
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
		if (curWeapaon) return MoveToInven(Character);
	case EItemPlace::INVEN:
		return MoveToSlot(Character);
	case EItemPlace::SLOT:
		return MoveToInven(Character);
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
	//인벤에서 장착을 하면 이상한 위치에 있던 문제를 이걸로 우선 해결.
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	return true; 
}

bool AC_MeleeWeapon::LegacyMoveToAround(AC_BasicCharacter* Character)
{
	if (!Character) return false;

	UC_InvenComponent* invenComp = Character->GetInvenComponent();
	//if (ItemDatas.ItemPlace == EItemPlace::SLOT)
	//TODO:여기 말고 다른 함수에서 때어 줘야 할 거 같음.
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	ItemDatas.ItemPlace = EItemPlace::AROUND;
	SetOwnerCharacter(nullptr);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	//if (ItemDatas.ItemPlace == EItemPlace::INVEN)
	invenComp->RemoveItemToMyList(this); //여기서 임시로 뺴주고 있는데 별로 안좋아 보임.
		//invenComp->AddItemToAroundList(this);
	//바닥 레이 캐스팅 받아와서 바닥에 아이템 생성하기.
	SetActorLocation(GetGroundLocation(Character) + RootComponent->Bounds.BoxExtent.Z);
	return true;
}

bool AC_MeleeWeapon::LegacyMoveToSlot(AC_BasicCharacter* Character)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();
	UC_InvenComponent* invenComp = Character->GetInvenComponent();

	AC_Item* unEquipItem = nullptr; //해제할 아이템

	AC_Item* inItem = nullptr; //장착할 아이템

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
	AttackCollider->SetCollisionEnabled(Enabled ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
}

void AC_MeleeWeapon::OnBodyColliderBeginOverlap
(
	UPrimitiveComponent*	OverlappedComponent,
	AActor*					OtherActor, 
	UPrimitiveComponent*	OtherComp,
	int32					OtherBodyIndex,
	bool					bFromSweep,
	const FHitResult&		SweepResult
)
{
	// TODO : 피격체에 데미지 주기
	UC_Util::Print("Pan AttackBodyColliderBeginOverlap", FColor::MakeRandomColor(), 10.f);

}

void AC_MeleeWeapon::OnOwnerCharacterPoseTransitionFin()
{
}

bool AC_MeleeWeapon::ExecuteAIAttack(AC_BasicCharacter* InTargetCharacter)
{
	if (!IsValid(InTargetCharacter))
	{
		UC_Util::Print("From AC_MeleeWeapon::ExecuteAIAttack : Invalid TargetCharacter!", FColor::Red, 10.f);
		return false;
	}
	
	if (OwnerCharacter->GetHandState() != EHandState::WEAPON_MELEE)
	{
		UC_Util::Print("From AC_MeleeWeapon::ExecuteAIAttack : Mismatched HandState", FColor::Red, 10.f);
		return false;
	}
	if (!ExecuteMlb_Started())
	{
		UC_Util::Print("From AC_MeleeWeapon::ExecuteAIAttack : AttackStrategy Failed!", FColor::Red, 10.f);
		return false; // 공격 시도 & 공격 성공 했는지 체크
	}

	// 공격 성공했다면 TargetCharacter 방면으로 회전 처리시키기
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation
	(
	this->OwnerCharacter->GetActorLocation(),
	InTargetCharacter->GetActorLocation()
	);

	LookAtRotation.Pitch = 0.f;
	LookAtRotation.Roll = 0.f;
	OwnerCharacter->SetActorRotation(LookAtRotation);
	
	return true;
}

