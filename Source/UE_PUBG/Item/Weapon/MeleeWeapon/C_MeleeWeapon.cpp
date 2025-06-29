// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/MeleeWeapon/C_MeleeWeapon.h"

#include "Item/Weapon/WeaponStrategy/C_MeleeWeaponStrategy.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"
#include "Components/CapsuleComponent.h"

#include "Components/ShapeComponent.h"
#include "Item/Equipment/C_EquipableItem.h"

#include "UObject/ConstructorHelpers.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Singleton/C_GameInstance.h"

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
	//ItemDataRef.ItemType = EItemTypes::MELEEWEAPON;
}

void AC_MeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

	AttackCollider = Cast<UShapeComponent>(GetDefaultSubobjectByName("AttackCollider"));
	if (!AttackCollider)
	{
		UC_Util::Print("From MeleeWeapon : GetDefaultSubobjectByName(AttackCollider) failed!", FColor::Red, 10.f);
		return;
	}
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

	//for (auto Element : OverlappedChara)
	//{
	//	
	//}
}

void AC_MeleeWeapon::InitializeItem(FName NewItemCode)
{
	Super::InitializeItem(NewItemCode);

	static const FString ContextString(TEXT("Pan Sound Lookup"));
	
	//TODO : 나중에 ItemManager를 통해 아이템을 모두 관리하게 되면 ItemManager를 통해서 GunSoundData 정의해 주기.
	//UDataTable* PanSoundDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Project_PUBG/Common/Item/ItemDataTables/DT_ThrowingWeaponSoundData.DT_ThrowingWeaponSoundData"));
	
	UC_GameInstance* GI = Cast<UC_GameInstance>(GetGameInstance());

	UDataTable* PanSoundDataTable = GI->GetDataTables()[EDataTableType::MeleeSound];

	if (PanSoundDataTable)
	{
		const FMeleeWeaponSoundDatas* ItemData = PanSoundDataTable->FindRow<FMeleeWeaponSoundDatas>(ItemCode, ContextString);
		if (ItemData)
		{
			MeleeWeaponSoundData = ItemData;  // 원본 참조 저장
		}
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

	switch (ItemPlace)
	{
	case EItemPlace::AROUND:
		if (curWeapaon) return MoveToInven(Character, this->GetItemCurStack());
	case EItemPlace::INVEN:
		return MoveToSlot(Character, this->GetItemCurStack());
	case EItemPlace::SLOT:
		return MoveToInven(Character, this->GetItemCurStack());
	default:
		break;
	}
	return false;
}
//
//bool AC_MeleeWeapon::LegacyMoveToInven(AC_BasicCharacter* Character)
//{
//	UC_InvenComponent* invenComp = Character->GetInvenComponent();
//
//	if (invenComp->GetMaxVolume() < invenComp->GetCurVolume() + this->ItemDataRef->ItemVolume) return false;
//
//	invenComp->AddItemToMyList(this);
//	//AttachToHolster(Character->GetMesh());
//	//인벤에서 장착을 하면 이상한 위치에 있던 문제를 이걸로 우선 해결.
//	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
//
//	return true; 
//}
//
//bool AC_MeleeWeapon::LegacyMoveToAround(AC_BasicCharacter* Character)
//{
//	if (!Character) return false;
//
//	UC_InvenComponent* invenComp = Character->GetInvenComponent();
//	//if (ItemDatas.ItemPlace == EItemPlace::SLOT)
//	//TODO:여기 말고 다른 함수에서 때어 줘야 할 거 같음.
//	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
//	ItemPlace = EItemPlace::AROUND;
//	SetOwnerCharacter(nullptr);
//	SetActorHiddenInGame(false);
//	SetActorEnableCollision(true);
//	//if (ItemDatas.ItemPlace == EItemPlace::INVEN)
//	invenComp->RemoveItemToMyList(this); //여기서 임시로 뺴주고 있는데 별로 안좋아 보임.
//		//invenComp->AddItemToAroundList(this);
//	//바닥 레이 캐스팅 받아와서 바닥에 아이템 생성하기.
//	SetActorLocation(GetGroundLocation(Character) + RootComponent->Bounds.BoxExtent.Z);
//	return true;
//}
//
//bool AC_MeleeWeapon::LegacyMoveToSlot(AC_BasicCharacter* Character)
//{
//	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();
//	UC_InvenComponent* invenComp = Character->GetInvenComponent();
//
//	AC_Item* unEquipItem = nullptr; //해제할 아이템
//
//	AC_Item* inItem = nullptr; //장착할 아이템
//
//	AC_Weapon* curWeapaon = equipComp->GetWeapons()[EWeaponSlot::MELEE_WEAPON];
//
//	if (IsValid(curWeapaon))
//	{
//		AC_Weapon* OutToSlotWeapon = nullptr;
//		AC_Item* FoundItem = nullptr;
//
//		int nextVolume = 0;
//
//		if (ItemPlace == EItemPlace::INVEN) 
//			nextVolume = invenComp->GetCurVolume() - ItemDataRef->ItemVolume + curWeapaon->GetItemDatas()->ItemVolume;
//		else
//			nextVolume = invenComp->GetCurVolume() + curWeapaon->GetItemDatas()->ItemVolume;
//
//		if (nextVolume > invenComp->GetMaxVolume()) return false;
//		else
//		{
//			unEquipItem = equipComp->SetSlotWeapon(EWeaponSlot::MELEE_WEAPON, this);
//			invenComp->AddItemToMyList(unEquipItem);
//			return true;
//		}
//
//	}
//	else
//	{
//		equipComp->SetSlotWeapon(EWeaponSlot::MELEE_WEAPON, this);
//		invenComp->RemoveItemToMyList(this);
//		return true;
//		//if (this->ItemDatas.ItemPlace == EItemPlace::INVEN)
//	}
//		
//	//return false;
//}

void AC_MeleeWeapon::OnAttackBegin()
{
	AttackCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AttackedCharacters.Empty();
}

void AC_MeleeWeapon::OnAttackEnd()
{
	AttackCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackedCharacters.Empty();
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
	static const float DAMAGE = 80.f;

	// 피격체에 데미지 주기
	AC_BasicCharacter* OverlappedCharacter = Cast<AC_BasicCharacter>(OtherActor);
	
	if (!OverlappedCharacter) return;
	if (OverlappedCharacter == OwnerCharacter) return;
	
	UC_Util::Print("Pan BeginOverlap with : " + OverlappedCharacter->GetCharacterName(), FColor::Red, 10.f);
	
	if (AttackedCharacters.Contains(OverlappedCharacter)) return; // 이미 현재 Attack wave에서 Damage를 준 Character일 때

	// MeleeWeapon의 경우, 조끼 착용 여부에 따른 Damage량 조정을 여기서 처리
	// 조끼피를 안닳게 일부러 처리할 예정

	AC_EquipableItem* EquippedVest = OverlappedCharacter->GetInvenComponent()->GetEquipmentItems()[EEquipSlot::VEST];
	float DamageReduceFactor = (!IsValid(EquippedVest)) ? 1.f : EquippedVest->GetDamageReduceFactor();

	FKillFeedDescriptor KillFeedDesc =
	{
		EDamageType::Weapon,
		this->OwnerCharacter,
		OverlappedCharacter,
		this,
		false,
		0
	};
	
	OverlappedCharacter->GetStatComponent()->TakeDamage(DAMAGE * DamageReduceFactor, KillFeedDesc);
	OverlappedCharacter->ActivateBloodParticle(OverlappedCharacter->GetMesh()->GetBoneLocation("Spine1"));
	AttackedCharacters.Add(OverlappedCharacter);

	if (!MeleeWeaponSoundData)
	{
		UC_Util::Print("MeleeWeaponSoundData nullptr", FColor::Red, 10.f);
		return;
	}
	if (!MeleeWeaponSoundData->ImpactSound)
	{
		UC_Util::Print("ImpactSound nullptr", FColor::Red, 10.f);
		return;
	}

	if (Cast<AC_Player>(OwnerCharacter))
		UGameplayStatics::PlaySound2D(this, MeleeWeaponSoundData->ImpactSound);
	else
		UGameplayStatics::PlaySoundAtLocation(this, MeleeWeaponSoundData->ImpactSound, GetActorLocation());

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

