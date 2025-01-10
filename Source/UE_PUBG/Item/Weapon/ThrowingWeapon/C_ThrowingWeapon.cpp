  // Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"

#include "Item/Weapon/WeaponStrategy/I_WeaponButtonStrategy.h"
#include "Item/Weapon/WeaponStrategy/C_ThrowingWeaponStrategy.h"

#include "Components/ShapeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "Components/SplineMeshComponent.h"
#include "Components/SplineComponent.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"

#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_InvenSystem.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenSystem.h"	
#include "Character/C_AnimBasicCharacter.h"

#include "HUD/C_HUDWidget.h"
#include "HUD/C_MainMapWidget.h"

#include "I_ExplodeStrategy.h"
#include "C_GrenadeExplode.h"
#include "C_FlashBangExplode.h"
#include "C_SmokeGrndExplode.h"

#include "Utility/C_Util.h"

#include "Singleton/C_GameSceneManager.h"

const float AC_ThrowingWeapon::UP_DIR_BOOST_OFFSET = 500.f;

int AC_ThrowingWeapon::ThrowingWeaponCount{};

TMap<EThrowableType, II_ExplodeStrategy*> AC_ThrowingWeapon::ExplodeStrategies{};

const TMap<EThrowableType, FString> AC_ThrowingWeapon::THROWABLETYPE_ITEMNAME_MAP =
{
	{EThrowableType::GRENADE,		"Grenade"},
	{EThrowableType::FLASH_BANG,	"FlashBang"},
	{EThrowableType::SMOKE,			"Smoke Grenade"},
};

USkeletalMeshComponent* AC_ThrowingWeapon::OwnerMeshTemp{};

AC_ThrowingWeapon::AC_ThrowingWeapon()
{
	WeaponButtonStrategy = CreateDefaultSubobject<AC_ThrowingWeaponStrategy>("ThrowingWeaponStrategy");

	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRootComponent"));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");

	Collider = CreateDefaultSubobject<UCapsuleComponent>("Capsule");
	Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(Collider);

	PathSpline = CreateDefaultSubobject<USplineComponent>("PredictedPathSpline");
	PredictedEndPoint = CreateDefaultSubobject<UStaticMeshComponent>("PredictedPathEndPointMesh");
	PredictedEndPoint->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PredictedEndPoint->SetVisibility(false);

	//ItemType ����.
	ItemDatas.ItemType = EItemTypes::THROWABLE;

	ExplosionSphere = CreateDefaultSubobject<USphereComponent>("ExplosionSphere");
}

void AC_ThrowingWeapon::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovement->Deactivate();

	ThrowingWeaponCount++;

	// Init ExplodeStrategies
	if (ExplodeStrategies.IsEmpty())
	{
		II_ExplodeStrategy* GrenadeExplode = NewObject<AC_GrenadeExplode>();
		GrenadeExplode->_getUObject()->AddToRoot(); // GC ����

		II_ExplodeStrategy* FlashBangExplode = NewObject<AC_FlashBangExplode>();
		FlashBangExplode->_getUObject()->AddToRoot();

		II_ExplodeStrategy* SmokeExplode = NewObject<AC_SmokeGrndExplode>();
		SmokeExplode->_getUObject()->AddToRoot();

		GAMESCENE_MANAGER->AddNonGCObject(GrenadeExplode->_getUObject());
		GAMESCENE_MANAGER->AddNonGCObject(FlashBangExplode->_getUObject());
		GAMESCENE_MANAGER->AddNonGCObject(SmokeExplode->_getUObject());

		ExplodeStrategies.Add(EThrowableType::GRENADE,		GrenadeExplode);
		ExplodeStrategies.Add(EThrowableType::FLASH_BANG,	FlashBangExplode);
		ExplodeStrategies.Add(EThrowableType::SMOKE,		SmokeExplode);
	}

	// Init Explode Strategy
	ExplodeStrategy = ExplodeStrategies[ThrowableType];
}

void AC_ThrowingWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	if (EndPlayReason == EEndPlayReason::Destroyed)
	{
		if (--ThrowingWeaponCount <= 0) // World�� ��ġ�� ������ ThrowingWeapon�� Destroy�Ǿ��� ��
		{
			if (OwnerMeshTemp)
			{
				OwnerMeshTemp->DestroyComponent();
				OwnerMeshTemp = nullptr;
			}

			if (!ExplodeStrategies.IsEmpty()) ExplodeStrategies.Empty(); // GC�� GameSceneManager���� ó��
		}
		return;
	}

	if (OwnerMeshTemp)
	{
		OwnerMeshTemp->DestroyComponent();
		OwnerMeshTemp = nullptr;
	}

	if (!ExplodeStrategies.IsEmpty()) ExplodeStrategies.Empty(); // GC�� GameSceneManager���� ó��
}

void AC_ThrowingWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
			
	if (!IsValid(OwnerCharacter))
		return;

	CurDrawMontage			= DrawMontages[OwnerCharacter->GetPoseState()];
	CurSheathMontage		= SheathMontages[OwnerCharacter->GetPoseState()];
	CurThrowProcessMontages = ThrowProcessMontages[OwnerCharacter->GetPoseState()];
}

bool AC_ThrowingWeapon::AttachToHolster(USceneComponent* InParent)
{
	// ��ô���� �ɱ����� �̾Ұ� ��ŷ�� ������ �� �ٽ� �������
	// ��ô���� ���������̱��� �̾Ҵٸ� ���� ��ġ�� ���� ��ô�� �׳� �ٴڿ� ����

	UC_Util::Print("AttachToHolster");

	SetActorHiddenInGame(true);
	

	ProjectileMovement->Deactivate();

	return AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		HOLSTER_SOCKET_NAME
	);
}

bool AC_ThrowingWeapon::AttachToHand(USceneComponent* InParent)
{
	//this->SetHidden(false);

	UC_Util::Print("AttachToHand");

	SetActorHiddenInGame(false);
	//SetActorHiddenInGame(true);


	ProjectileMovement->Deactivate();
	ClearSpline();

	OwnerCharacter->SetHandState(EHandState::WEAPON_THROWABLE);
	
	// Self init
	bIsCharging = false;
	bIsOnThrowProcess = false;

	return AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
		EQUIPPED_SOCKET_NAME
	);
}

void AC_ThrowingWeapon::PickUpItem(AC_BasicCharacter* Character)
{
	//���⼭�ϴ� �뷮üũ�� �κ����� �̹� �ѹ� ó�� �Ǿ����� Ȥ�ø��� �־� ���������� Ȯ���� ������ ��.
	//�κ����� üũ���� �ʰ� �����ۿ��� üũ�ϴ� ������� ���� �ҵ�.
	
	//EquipToCharacter(Character);
	LegacyMoveToSlot(Character);
}

void AC_ThrowingWeapon::DropItem(AC_BasicCharacter* Character)
{
	//TODO : �������� ����(Attach)�Ǿ��� ���¸� �����ϴ� �۾��� ���� ó�� ����
	//TODO : �����ؼ� ������ ��� ���� �������־����.
	ItemDatas.ItemPlace = EItemPlace::AROUND;
	SetOwnerCharacter(nullptr);               //OwnerCharacter ����
	SetActorHiddenInGame(false);			  //����� ���̵��� Hidden ����.
	SetActorEnableCollision(true);			  //Overlap���� �ϵ��� Collision On
	Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	//�ٴ� ���� ĳ���� �޾ƿͼ� �ٴڿ� ������ �����ϱ�.
	SetActorLocation(GetGroundLocation(Character) + RootComponent->Bounds.BoxExtent.Z);
}

//void AC_ThrowingWeapon::SetItemStack(uint8 ItemStack)
//{
//	ItemDatas.ItemCurStack = ItemStack;
//}

void AC_ThrowingWeapon::EquipToCharacter(AC_BasicCharacter* Character)
{
	UC_InvenComponent* invenComp = Character->GetInvenComponent();

	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();

	AC_Item* unEquipItem = nullptr;

	AC_Item* inItem = nullptr;

	//��ôĭ�� ����ִ��� �˻�
	if (equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON])
	{
		//������ ��ô���� �����ϰ� �ִٸ� ����.
		uint8 nextVolume = 
			invenComp->GetCurVolume() 
			+ equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON]->GetItemDatas().ItemVolume
			- ItemDatas.ItemVolume;

		if (nextVolume > invenComp->GetMaxVolume()) //��ü�� �κ��� �뷮�� �ʰ��Ѵٸ� ��ü �Ұ���, 
		{
			//�κ��� ������ üũ�ؼ� �κ�����
			if (invenComp->CheckVolume(this))
			{
				inItem = invenComp->FindMyItem(this);
				if (inItem)
				{
					inItem->AddItemStack();
					SetActorHiddenInGame(true);
					SetActorEnableCollision(false);
					this->Destroy();
					return; //�κ����� �������� �����Ƿ� ����.
				}
			}
		}
	

		//�ʰ����� �ʴ´ٸ� ��ô�� ��ü. unEquipItem = ��ü���� ������.
		unEquipItem = equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, this);

		//���������Ͷ� Ȯ�� �ѹ� �� ��. 
		inItem = invenComp->FindMyItem(unEquipItem);

		if (inItem)
		{
			//��ü���� �������� �κ��� �����Ѵٸ�. �κ��� �������� ������ + 1.
			inItem->AddItemStack();
			unEquipItem->SetActorHiddenInGame(true);
			unEquipItem->SetActorEnableCollision(false);
			unEquipItem->Destroy();
		}
		else
		{
			//��ü���� �������� �κ��� �������� �ʴ´ٸ�. �κ����� �������� �̵�.
			invenComp->AddItemToMyList(unEquipItem);

			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);
		}
	}
	else
	{
		//������ ��ô���� �������� �ʴ´ٸ� ����. �ٷ� ����.
		equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, this);
		
		//AroundItemList���� �ش� �������� �Ȼ�����ٸ� �����ġ �� ��.
		//invenComp->RemoveAroundItem(this);
		
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
	}
}

bool AC_ThrowingWeapon::LegacyMoveToInven(AC_BasicCharacter* Character)
{

	UC_InvenComponent*    invenComp = Character->GetInvenComponent();
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();

	uint8 ItemStackCount = invenComp->LoopCheckVolume(this);

	if (ItemStackCount == 0)
	{
		UC_Util::Print("Not Enough Volume");
		return false; //�κ��� ���� �� �ִ� �������� ������ 0 �̸� ���� �� �����Ƿ� return false;
	}

	AC_Item* FoundItem = invenComp->FindMyItem(this); //�κ��� ���� �������� ã�ƿ�, ���ٸ� nullptr;

	/* �� ������ ��� MoveToInven ������ ��Ȳ */

	// ���� �տ� ����ִ� ��ô���� this��� HandState NONE���� ��ȯ
	if (equipComp->GetCurWeapon() == this) equipComp->ChangeCurWeapon(EWeaponSlot::NONE);

	if (ItemDatas.ItemCurStack == ItemStackCount)
	{
		//������ ���θ� �κ��� ���� �� ���� ��.
		if (IsValid(FoundItem))
		{
			//�κ��� �ش� �������� ���� �� ��.
			FoundItem->SetItemStack(FoundItem->GetItemDatas().ItemCurStack + ItemStackCount);
			//invenComp->GetCurVolume() += FoundItem->GetItemDatas().ItemVolume * ItemStackCount;
			//TODO : destroy�� �ص� �ܻ��� ���°��� ����ؼ� �س��� ���� ���̵� �ܻ��� �ȳ��´ٸ� ���� ��.
			invenComp->AddInvenCurVolume(this->ItemDatas.ItemVolume * ItemStackCount);

			this->SetActorEnableCollision(false);
			this->SetActorHiddenInGame(true);

			this->Destroy();
			return true;
		}
		else
		{
			//�κ��� �ش� �������� �������� ���� ��.
			invenComp->AddItemToMyList(this);
			//�ΰ��ӿ��� ���̴� �Ͱ� collision���� ������ �ӽ÷� ����.
			//this->SetActorEnableCollision(false);
			this->SetActorHiddenInGame(true);
			//���� �� ����. �̰ɷ� ���� �������� �������� �ȳ����ٸ� �ٸ� ��� ���->ToInven���� SetActorEnableCollision�� ���ְ� ������ Ȥ�� ToAround���� ���ֱ�.
			Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			return true;
		}
	}
	else
	{
		//�������� �Ϻθ� �κ��� ���� �� ���� ��.
		if (IsValid(FoundItem))
		{
			     this->SetItemStack(ItemDatas.ItemCurStack - ItemStackCount);
			FoundItem->SetItemStack(FoundItem->GetItemDatas().ItemCurStack + ItemStackCount);

			invenComp->AddInvenCurVolume(this->ItemDatas.ItemVolume * ItemStackCount);

			return true;
		}
		else
		{
			AC_Weapon* NewItem = Cast<AC_Weapon>(SpawnItem(Character));//������ ���� ����
			NewItem->SetItemStack(ItemStackCount);
			   this->SetItemStack(ItemDatas.ItemCurStack - ItemStackCount);

			invenComp->AddItemToMyList(NewItem);

			NewItem->SetActorHiddenInGame(true);
			//collider ���� ���� �߰��ؾ� �� �� ����.
			//���� �߰��ؾ� �ȴٸ� MoveToInven���� SetActorEnableCollision�� ���ְ� ���� �� ���ִ� �������.
			return true;
		}
	}
}

bool AC_ThrowingWeapon::LegacyMoveToAround(AC_BasicCharacter* Character)
{
	if (!Character) return false;

	UC_EquippedComponent* EquippedComponent = Character->GetEquippedComponent();

	// Slot�� ������ ���⸦ MoveToAroundó���ϴ� ��Ȳ
	if (EquippedComponent->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON] == this)
	{
		// ���� ���� �տ� ��� �ִ� ���⸦ MoveToAroundó�� ���� ��
		if (EquippedComponent->GetCurWeapon() == this)
			EquippedComponent->ChangeCurWeapon(EWeaponSlot::NONE);
		
		EquippedComponent->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, nullptr);
	}

	if (this->GetItemDatas().ItemPlace == EItemPlace::INVEN)
		Character->GetInvenComponent()->RemoveItemToMyList(this);
	else if (this->GetItemDatas().ItemPlace == EItemPlace::SLOT)
	{
		Character->GetEquippedComponent()->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, nullptr);
		//this->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	}
	
	//DetachmentItem();

	//TODO: �����ؼ� ������ ��� ���� �������־����.
	ItemDatas.ItemPlace = EItemPlace::AROUND;
	SetOwnerCharacter(nullptr);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	//�ٴ� ���� ĳ���� �޾ƿͼ� �ٴڿ� ������ �����ϱ�.
	SetActorLocation(GetGroundLocation(Character) + RootComponent->Bounds.BoxExtent.Z);
	
	//SetActorRotation(FQuat(0,0,0));

	return true;
}

bool AC_ThrowingWeapon::LegacyMoveToSlot(AC_BasicCharacter* Character)
{
	UC_EquippedComponent*	equipComp = Character->GetEquippedComponent();
	UC_InvenComponent*		invenComp = Character->GetInvenComponent();
	AC_Weapon*				curWeapon = equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON];

	AC_Weapon* curWeapaon = equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON];

	// ���Կ� ���Ⱑ �����Ǿ� ���� ��
	if (IsValid(curWeapon))
	{
		AC_ThrowingWeapon* CurSlotThrowWeapon = Cast<AC_ThrowingWeapon>(curWeapon);

		if (!CurSlotThrowWeapon)
		{
			UC_Util::Print("From AC_ThrowingWeapon::MoveToSlot : Casting failed!", FColor::Red, 10.f);
			return false;
		}

		// ������ slot�� ������ ThrowableWeapon�� ThrowProcess ���̶�� return false
		if (CurSlotThrowWeapon->GetIsOnThrowProcess()) return false;

		AC_Weapon* PrevSlotWeapon = nullptr;
		AC_Item* FoundItem		  = nullptr;
		//int nextVolume = invenComp->GetCurVolume() - ItemDatas.ItemVolume + curWeapaon->GetItemDatas().ItemVolume; //�̰� �κ��� �����ϴ� �������� �ű涧�� ��ȿ �Ѱ� ������?
		int nextVolume = 0;

		nextVolume = (this->ItemDatas.ItemPlace == EItemPlace::INVEN) ? invenComp->GetCurVolume() -
																		ItemDatas.ItemVolume +
																		curWeapon->GetItemDatas().ItemVolume 
																		:
																		invenComp->GetCurVolume() + curWeapon->GetItemDatas().ItemVolume;

		if (nextVolume > invenComp->GetMaxVolume())
		{
			UC_Util::Print("Not Enough Volume");
			return false;
		}

		//SetActorEnableCollision(false);
		if (ItemDatas.ItemCurStack == 1)
		{
			if (this->ItemDatas.ItemPlace == EItemPlace::INVEN)       invenComp->RemoveItemToMyList(this);//�Ƹ� InvenUI�� �ʱ�ȭ �����ִ� �۾��� �߰������� �ʿ��Ұ�.
			else if (this->ItemDatas.ItemPlace == EItemPlace::AROUND) invenComp->RemoveItemToAroundList(this);

			PrevSlotWeapon = equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, this);
		}
		else
		{
			AC_Weapon* InToSlotWeapon = Cast<AC_Weapon>(SpawnItem(Character));
			InToSlotWeapon->SetItemStack(1);
			this->SetItemStack(ItemDatas.ItemCurStack - 1);

			//if (ItemDatas.ItemPlace == EItemPlace::AROUND) return this->LegacyMoveToInven(Character);//�ϳ��� ���� ������Ų �������� MoveToInven���� �κ��� �ִ� �۾� ����.
			//else if (ItemDatas.ItemPlace == EItemPlace::INVEN) return true;
			//else return false;

			PrevSlotWeapon = equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, InToSlotWeapon);

		}

		// �տ� �� slot ���⸦ ���� ���� �� ���� ó��
		if (Character->GetHandState() == EHandState::WEAPON_THROWABLE)
		{
			// ���� ��ô�� AttachToHolster ó���ϰ� ���ο� ��ô�� ������
			UC_Util::Print("HandState Throwable set slot Exception Handling", FColor::Red, 10.f);
			PrevSlotWeapon->AttachToHolster(Character->GetMesh());
			equipComp->SetNextWeaponType(EWeaponSlot::THROWABLE_WEAPON);
			Character->PlayAnimMontage(this->GetCurDrawMontage());
		}

		//if (OutToSlotWeapon->MoveToInven(Character)) return true;

		FoundItem = invenComp->FindMyItem(PrevSlotWeapon); //�κ��� �ش� �������� �����ϴ��� Ȯ��

		if (IsValid(FoundItem))
		{
			//FoundItem->SetItemStack(FoundItem->GetItemDatas().ItemStack + 1);
			FoundItem->AddItemStack();
			//TODO:�ܻ��� ����ٸ� �߰� �۾� �ʿ�.
			PrevSlotWeapon->Destroy();
			return true;
		}

		invenComp->AddItemToMyList(PrevSlotWeapon);
		return true;
	}

	//���Կ� �������� ���� ��

	if (ItemDatas.ItemCurStack == 1)
	{
		//this�� stack�� 1 �̶�� ����.
		//this�� ���Կ� ���� �ϰ� ��Ͽ��� ����

		if      (ItemDatas.ItemPlace == EItemPlace::AROUND) invenComp->RemoveItemNearList(this);
		else if (ItemDatas.ItemPlace == EItemPlace::INVEN)  invenComp->RemoveItemToMyList(this);

		equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, this);
		return true;
	}

	//this�� ������ 1�� �ƴ� ��
	
	//this�� ���� �����ؼ� �ϳ��� �����ϰ� �������� ������ ����
	AC_Weapon* NewWeapon = Cast<AC_Weapon>(SpawnItem(Character));
	NewWeapon->SetItemStack(1);
	     this->SetItemStack(ItemDatas.ItemCurStack - 1);
	
	equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, NewWeapon); //���Կ� ����. �Ʒ��� NewWeapon�� �ٸ�.

	return  (ItemDatas.ItemPlace == EItemPlace::AROUND) ? this->MoveToInven(Character) : //�ϳ��� ���� ������Ų �������� MoveToInven���� �κ��� �ִ� �۾� ����.
			(ItemDatas.ItemPlace == EItemPlace::INVEN)  ? true : false;
}

bool AC_ThrowingWeapon::Interaction(AC_BasicCharacter* Character)
{
	//if (Character->GetInvenSystem()->GetInvenUI()->GetIsDragging())
	//{
	//	switch (ItemDatas.ItemPlace)
	//	{
	//	case EItemPlace::AROUND:
	//		break;
	//	default:
	//		break;
	//	}
	//	
	//	return MoveToAround(Character);
	//}
	AC_Weapon* curWeapaon = Character->GetEquippedComponent()->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON];

	//if (curWeapaon)
	//{
	//	return MoveToInven(Character);
	//}
	//else
	//{
	//	return MoveToSlot(Character);
	//}

	switch (ItemDatas.ItemPlace)
	{
	case EItemPlace::SLOT:

	case EItemPlace::AROUND:
		if (curWeapaon) return MoveToInven(Character);
	case EItemPlace::INVEN:
		return MoveToSlot(Character);
		break;
	default:
		break;
	}
	return false;
	//if (Character)
	//{
	//	MoveToSlot(Character);
	//	return true;
	//}
	////MoveToSlot(Character);

	//return false;
}

AC_Item* AC_ThrowingWeapon::SpawnItem(AC_BasicCharacter* Character)
{
	FActorSpawnParameters SpawnParams;
	//SpawnParams.Owner = Character;
	//location, rotation�� this�� ���� ���� �͵� ����, �ֳ��ϸ� ���� �̻��ϰ� ���󰡴� ������ �̰��� �� �� ����. -> ����ź�� ������ �浹ü�� ��������.
	AC_Weapon* SpawnItem = GetWorld()->SpawnActor<AC_Weapon>(this->GetClass(), Character->GetActorLocation() - FVector(0,0,50), Character->GetActorRotation(), SpawnParams);
	//SpawnItem->SetItemStack(1);
	//SpawnItem->SetActorHiddenInGame(true);
	SpawnItem->SetActorEnableCollision(false);//������ �� ������ OverlapBegine�� �����ؼ� �켱 ������.
	return SpawnItem;
}

bool AC_ThrowingWeapon::MoveSlotToAround(AC_BasicCharacter* Character)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : �Ⱦ��°� �����ϱ�.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : �Ⱦ��°� �����ϱ�.

	//Slot�� �ִٴ°� ������ ���¶�� ��. Around�� ���ٴ°� ������ �����ϰ� �������� ���� �����ٴ� ��.
	AC_Weapon* curWeapon = equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON];
	
	if (curWeapon != this) return false; //������ �������� �ڽ��� �ƴ϶�� return.

	equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, nullptr); //������ �������� �ڽ��̸�, ���������� ����.

	DropItem(Character);
	return true;
}

bool AC_ThrowingWeapon::MoveSlotToInven(AC_BasicCharacter* Character)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : �Ⱦ��°� �����ϱ�.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : �Ⱦ��°� �����ϱ�.
	//if (invenComp->GetMaxVolume() > invenComp->GetCurVolume() + GetOnceVolume())
	if (!invenComp->CheckVolume(this)) return false; //�κ��� this�� ���� curVolume > MaxVolume�� �ȴٸ� return.

	AC_Weapon* curWeapon = equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON];

	if (curWeapon != this) return false; //������ �������� �ڽ��� �ƴ϶�� return.

	equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, nullptr); //������ �������� �ڽ��̸�, ���������� ����.

	invenComp->AddItemToMyList(this);//���� ������ �������� �� ������ ����Ʈ�� �߰�.
	
	return true;
}

bool AC_ThrowingWeapon::MoveSlotToSlot(AC_BasicCharacter* Character)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : �Ⱦ��°� �����ϱ�.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : �Ⱦ��°� �����ϱ�.

	//�Ⱦ�.
	return false;
}

bool AC_ThrowingWeapon::MoveInvenToAround(AC_BasicCharacter* Character)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : �Ⱦ��°� �����ϱ�.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : �Ⱦ��°� �����ϱ�.

	if (!invenComp->FindMyItem(this)) return false;

	invenComp->RemoveItemToMyList(this);				 //�� ������ ����Ʈ���� ������ ����.
														 
	//invenComp->AddInvenCurVolume(-this->GetAllVolume()); //������ �����۸�ŭ curVolume �����ϱ�. TODO : Inven���� ������ ���� �� ���� ����� üũ�ϱ�.

	DropItem(Character);

	return true;
}

bool AC_ThrowingWeapon::MoveInvenToInven(AC_BasicCharacter* Character)
{
	//UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : �Ⱦ��°� �����ϱ�.
	//UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : �Ⱦ��°� �����ϱ�.

	//�Ⱦ�
	return false;
}

bool AC_ThrowingWeapon::MoveInvenToSlot(AC_BasicCharacter* Character)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : �Ⱦ��°� �����ϱ�.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : �Ⱦ��°� �����ϱ�.

	AC_Weapon* curWeapon = equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON];

	if (curWeapon)
	{
		float prevVolume = invenComp->GetCurVolume() + curWeapon->GetOnceVolume() - this->GetOnceVolume();
		if (prevVolume > invenComp->GetMaxVolume()) return false; //��ü�ϴ� �������� �κ��� �����鼭 MaxVolume�� ������ return.
	}

	if (this->GetItemDatas().ItemCurStack == 1)
	{
		curWeapon = equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, this);
		invenComp->RemoveItemToMyList(this);
	}
	else
	{
		this->DeductItemStack();
		AC_ThrowingWeapon* SwapItem = Cast<AC_ThrowingWeapon>(SpawnItem(Character));
		SwapItem->SetItemStack(1);
		curWeapon = equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, SwapItem);
	}

	if (Character->GetHandState() == EHandState::WEAPON_THROWABLE)
		AttachToHand(Character->GetMesh());

	Character->PlayAnimMontage(this->GetCurDrawMontage());
	invenComp->AddItemToMyList(curWeapon);//���ο��� �Ű����� nullptr�� ������ return���ѹ���. TODO : curWeapon�� ������ �ڿ� equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON]�� ���� �ٲ�����Ƿ� �������� �ϸ� ������ ����. Ȯ�� �� ��.
	//if (curWeapon) //TODO : InvenComp->AddItemToMyList(nullptr)�� ���� ����� Ȱ��ȭ 
	
	return true;
}

bool AC_ThrowingWeapon::MoveAroundToAround(AC_BasicCharacter* Character)
{
	//UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : �Ⱦ��°� �����ϱ�.
	//UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : �Ⱦ��°� �����ϱ�.

	//�Ⱦ�
	return false;
}

bool AC_ThrowingWeapon::MoveAroundToInven(AC_BasicCharacter* Character)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : �Ⱦ��°� �����ϱ�.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : �Ⱦ��°� �����ϱ�.

	uint8 ItemStackCount = invenComp->LoopCheckVolume(this); //������Stack�� ����� �κ��� ���� �� �ִ°�?

	if (ItemStackCount == 0)
	{
		UC_Util::Print("Not Enough Volume");
		return false; //�κ��� ���� �� �ִ� �������� ������ 0 �̸� ���� �� �����Ƿ� return false;
	}

	if (ItemStackCount == this->GetItemDatas().ItemCurStack)
	{
		//�������� ���� �κ��� ���� �� �ִ� ���.
		invenComp->AddItemToMyList(this);
		return true;
	}
	else 
	{
		//�������� ���� ���� �� ���� ���.
		this->SetItemStack(GetItemDatas().ItemCurStack - ItemStackCount);//���� ��ü�� stack�� ����
		AC_Weapon* SpawnedItem = Cast<AC_Weapon>(SpawnItem(Character));  //������ ������ ��ü�� ����
		SpawnedItem->SetItemStack(ItemStackCount);						 //������ ������ stack�� ����
		invenComp->AddItemToMyList(SpawnedItem);						 //inven�� �߰�.
		return true;
	}
}

bool AC_ThrowingWeapon::MoveAroundToSlot(AC_BasicCharacter* Character)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : �Ⱦ��°� �����ϱ�.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : �Ⱦ��°� �����ϱ�.

	AC_ThrowingWeapon* curWeapon = Cast<AC_ThrowingWeapon>(equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON]);

	if (curWeapon)
	{
		if (!curWeapon->MoveSlotToInven(Character)) //�̰����� curWeapon�� ������ ������.
			curWeapon->MoveSlotToAround(Character);
	}

	if (this->GetItemDatas().ItemCurStack == 1)
	{
		equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, this);
		invenComp->RemoveItemToAroundList(this);
	}
	else
	{
		this->DeductItemStack();
		AC_ThrowingWeapon* SwapItem = Cast<AC_ThrowingWeapon>(SpawnItem(Character));
		SwapItem->SetItemStack(1);
		equipComp->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, SwapItem);
	}
	return true;
}

void AC_ThrowingWeapon::OnRemovePinFin()
{
	OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowReadyMontage);
}

void AC_ThrowingWeapon::OnThrowReadyLoop()
{
	// Player HUD Opened ���� ����ó��
	if (AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharacter))
	{
		// UI â�� ������ ��
		if (OwnerPlayer->GetInvenSystem()->GetInvenUI()->GetIsPanelOpened() ||
			OwnerPlayer->GetHUDWidget()->GetMainMapWidget()->GetIsPanelOpened())
		{
			ClearSpline();

			if (bIsCooked) // cooking �Ǿ��ٸ� ��������
			{
				OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowMontage);
				return;
			}

			// Cooking �Ǿ����� �ʴٸ� �ٽ� Idle �ڼ��� ���ƿ�
			OwnerPlayer->GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, CurThrowProcessMontages.ThrowReadyMontage.AnimMontage);
			bIsOnThrowProcess	= false;
			bIsCharging			= false;
			return;
		}
	}

	if (bIsCharging)
	{
		// Charging �� ó��
		HandlePredictedPath();
		return;
	}

	// Charging�� Ǯ���� �� ���� ������ ���� ����
	ClearSpline();
	OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowMontage);
}

void AC_ThrowingWeapon::OnThrowThrowable()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	//OwnerCharacter->GetEquippedComponent()->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, nullptr);
	// Direction ���ϴ� ��� 1
	/*FVector ActorForward = FRotationMatrix(OwnerCharacter->GetActorRotation()).GetUnitAxis(EAxis::X);

	UC_AnimBasicCharacter* OwnerAnim = Cast<UC_AnimBasicCharacter>(OwnerCharacter->GetMesh()->GetAnimInstance());
	
	if (!IsValid(OwnerAnim))
	{
		FString DebugMessage = "AC_ThrowingWeapon::OnThrowThrowable : OwnerAnim not valid!";
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *DebugMessage);

		Direction = ActorForward;
	}
	else
	{
		FRotator SpineRotation	  = OwnerAnim->GetCSpineRotation();
		FVector SpineForward	  = FRotationMatrix(SpineRotation).GetUnitAxis(EAxis::X);
		FVector CombinedDirection = FRotationMatrix(OwnerCharacter->GetActorRotation()).TransformVector(SpineForward);
		Direction = CombinedDirection;
	}*/

	UpdateProjectileLaunchValues();

	//UC_Util::Print(ProjStartLocation);

	Collider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	//ECollisionResponse Response = Collider->GetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic);

	//if (Response == ECollisionResponse::ECR_Block)
	//	UC_Util::Print("Response Block", FColor::Red, 10.f);


	this->SetActorLocation(ProjStartLocation);
	
	ProjectileMovement->Velocity = ProjLaunchVelocity;
	ProjectileMovement->Activate();

	if (!bIsCooked) StartCooking();
}

void AC_ThrowingWeapon::OnThrowProcessEnd()
{
	//UC_Util::Print("OnThrowProcessEnd", FColor::Cyan, 5.f);

	bIsOnThrowProcess = false;

	bIsCharging = false;

	SetActorEnableCollision(true);//���� �� �� ���� �浹 ���ֱ�.

	// Ready ���� ProcessEnd�� �� �� �ֱ� ������ Predicted Path spline ��� �����ֱ�
	ClearSpline();

	// ���� Throw AnimMontage ���߱� (�켱���� ������ ����� ��)
	//OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, CurThrowProcessMontages.ThrowMontage.AnimMontage);

	UAnimInstance* OwnerAnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();

	//OwnerAnimInstance->StopAllMontages(0.2f);

	// �߰��� �ڼ��� �ٲ� �� �ֱ� ������ ��� �ڼ��� AnimMontage playing üũ
	for (uint8 p = 0; p < static_cast<uint8>(EPoseState::POSE_MAX); p++)
	{
		EPoseState	  PoseState	   = static_cast<EPoseState>(p);
		UAnimMontage* ThrowMontage = ThrowProcessMontages[PoseState].ThrowMontage.AnimMontage;
		UAnimMontage* ReadyMontage = ThrowProcessMontages[PoseState].ThrowReadyMontage.AnimMontage;

		UAnimMontage* TargetMontage = OwnerAnimInstance->Montage_IsPlaying(ThrowMontage) ? ThrowMontage :
									  OwnerAnimInstance->Montage_IsPlaying(ReadyMontage) ? ReadyMontage : nullptr;

		if (TargetMontage)
		{
			OwnerAnimInstance->Montage_Stop(0.2f, TargetMontage);
			break;
		}
	}

	UC_EquippedComponent* OwnerEquippedComponent = OwnerCharacter->GetEquippedComponent();

	// ���� ��ô�� ���� ���� �ٷ� �ϱ� -> �̹� ���� ���⿡ ���� PoseTransitionEnd Delegate ���� �̷����
	AC_BasicCharacter* PrevOwnerCharacter = OwnerCharacter;

	OwnerEquippedComponent->SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, nullptr);

	// ������ ������ ��ô�� �ִ��� ����
	AC_Item* ThrowWeapon = OwnerCharacter->GetInvenComponent()->FindMyItem(this);
	if (ThrowWeapon)
	{
		AC_ThrowingWeapon* TargetThrowWeapon = Cast<AC_ThrowingWeapon>(ThrowWeapon);

		if (!TargetThrowWeapon)
		{
			UC_Util::Print("OnProcessEnd Change to Last New Throwing Weapon casting failed!", FColor::Red, 10.f);
			return;
		}
		TargetThrowWeapon->LegacyMoveToSlot(PrevOwnerCharacter);

		// �ٷ� ���� ��ô�� ������
		OwnerEquippedComponent->SetNextWeaponType(EWeaponSlot::THROWABLE_WEAPON);
		PrevOwnerCharacter->PlayAnimMontage(TargetThrowWeapon->GetCurDrawMontage());
		return;
	}

	// �ٸ� ��ô�� ����
	for (auto& Pair : THROWABLETYPE_ITEMNAME_MAP)
	{
		if (Pair.Key == this->ThrowableType) continue;

		ThrowWeapon = OwnerCharacter->GetInvenComponent()->FindMyItem(Pair.Value);

		UC_Util::Print("1");

		if (!ThrowWeapon) continue;

		UC_Util::Print("2");

		AC_ThrowingWeapon* TargetThrowWeapon = Cast<AC_ThrowingWeapon>(ThrowWeapon);

		if (!TargetThrowWeapon)
		{
			UC_Util::Print("OnProcessEnd Change to Last New Throwing Weapon casting failed!", FColor::Red, 10.f);
			return;
		}

		TargetThrowWeapon->LegacyMoveToSlot(PrevOwnerCharacter);

		// �ٷ� ���� ��ô�� ������
		OwnerEquippedComponent->SetNextWeaponType(EWeaponSlot::THROWABLE_WEAPON);
		PrevOwnerCharacter->PlayAnimMontage(TargetThrowWeapon->GetCurDrawMontage());
		return;
	}

	//���� �ִ� ��ô���� ���ٸ� �ֹ���1, �ֹ���2 ������ ���� �õ�, ���ٸ� UnArmed�� ���·� ���ư���
	if (OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::MAIN_GUN))		return;
	if (OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::SUB_GUN))		return;

	OwnerEquippedComponent->ChangeCurWeapon(EWeaponSlot::NONE);

}

void AC_ThrowingWeapon::StartCooking()
{
	bIsCooked = true;

	// TODO : Ready ���¿��� Cooking �����ϸ� ���� �ð� HUD ����
	//UC_Util::Print("Throwable Starts cooking");

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AC_ThrowingWeapon::Explode, CookingTime, false);
}

bool AC_ThrowingWeapon::ReleaseOnGround()
{
	
	if (!bIsCooked) return false;						// Cooking�� �ȵǾ��ٸ� (���������̰� ���ư��� �ʾҴٸ�)
	if (ProjectileMovement->IsActive()) return false;	// �̹� �������ٸ�

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	Collider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	ProjectileMovement->Velocity = FVector(0.f, 0.f, 0.f); // 0.f�� �ӷ����� ������
	ProjectileMovement->Activate();

	OnThrowProcessEnd();

	return true;
}

void AC_ThrowingWeapon::Explode()
{
	if (!ExplodeStrategy)
	{
		UC_Util::Print("From AC_ThrowingWeapon::Explode : Explode Strategy nullptr!", FColor::Red, 10.f);
		return;
	}

	bool Exploded = ExplodeStrategy->UseStrategy(this);

	if (GetAttachParentActor()) ReleaseOnGround(); // �տ��� ���� ������ �ʾ��� ��
		
	//if (Exploded) this->Destroy();
	if (Exploded)
	{
		this->SetActorHiddenInGame(true);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
			{
				this->Destroy();
			}, 10.f, false);
	}
	else UC_Util::Print("Not Exploded!", FColor::Red, 10.f);
}

FVector AC_ThrowingWeapon::GetPredictedThrowStartLocation()
{
	if (!IsValid(OwnerMeshTemp))
	{
		OwnerMeshTemp = NewObject<USkeletalMeshComponent>(OwnerCharacter);

		if (!IsValid(OwnerMeshTemp))
		{
			UC_Util::Print("From AC_ThrowingWeapon::GetPredictedThrowStartLocation : OwnerTempMesh Not inited!");
			return FVector::ZeroVector;
		}

		OwnerMeshTemp->SetVisibility(false);
		OwnerMeshTemp->RegisterComponent();
		OwnerMeshTemp->SetSkeletalMesh(OwnerCharacter->GetMesh()->SkeletalMesh);
		OwnerMeshTemp->SetAnimInstanceClass(OwnerCharacter->GetMesh()->GetAnimInstance()->GetClass());
		OwnerMeshTemp->SetWorldTransform(OwnerCharacter->GetMesh()->GetComponentTransform());
	}

	// �ڼ��� �´� Montage�� ��������� �ʴٸ�, �ش� Montage�� ���� �ڿ� ���߱�
	static EPoseState PredictedPoseState = EPoseState::POSE_MAX;

	UAnimInstance* MeshAnimInstance = OwnerMeshTemp->GetAnimInstance();

	// ���� posing�� �ٸ��ٸ� ���� ��� �������� �޶���
	
	float ThrowThrowableTime{};

	for (const FAnimNotifyEvent& NotifyEvent : CurThrowProcessMontages.ThrowMontage.AnimMontage->Notifies)
	{
		if (NotifyEvent.NotifyName == "AN_OnThrowThrowable_C")
		{
			ThrowThrowableTime = NotifyEvent.GetTime();
			break;
		}
	}

	if (PredictedPoseState != OwnerCharacter->GetPoseState() ||
		MeshAnimInstance->Montage_GetPosition(CurThrowProcessMontages.ThrowMontage.AnimMontage) != ThrowThrowableTime)
	{
		PredictedPoseState = OwnerCharacter->GetPoseState();
		
		MeshAnimInstance->Montage_Play(CurThrowProcessMontages.ThrowMontage.AnimMontage);
		MeshAnimInstance->Montage_SetPosition(CurThrowProcessMontages.ThrowMontage.AnimMontage, ThrowThrowableTime);
		MeshAnimInstance->Montage_Pause();

		OwnerMeshTemp->TickPose(GetWorld()->GetDeltaSeconds(), true);
		OwnerMeshTemp->RefreshBoneTransforms();
		OwnerMeshTemp->RefreshFollowerComponents();
		OwnerMeshTemp->UpdateComponentToWorld();
	}

	OwnerMeshTemp->SetWorldTransform(OwnerCharacter->GetMesh()->GetComponentTransform());

	return OwnerMeshTemp->GetSocketLocation(THROW_START_SOCKET_NAME);
}

void AC_ThrowingWeapon::DrawDebugPredictedPath()
{
	static const float MaxSimTime	= 1.0f; // Max time to simulate
	static const float TimeStep		= 0.1f; // Time step for simulation
	FVector Gravity					= GetWorld()->GetGravityZ() * FVector::UpVector;

	TArray<FVector> PathPoints{};

	FVector CurrentLocation = GetPredictedThrowStartLocation();
	FVector CurrentVelocity = ProjLaunchVelocity;

	PathPoints.Add(CurrentLocation);

	for (float Time = 0; Time < MaxSimTime; Time += TimeStep)
	{
		FVector NextLocation = CurrentLocation + CurrentVelocity * TimeStep + 0.5f * Gravity * TimeStep * TimeStep;
		CurrentVelocity += Gravity * TimeStep;

		PathPoints.Add(NextLocation);
		CurrentLocation = NextLocation;

		if (PathPoints.Num() > 1)
		{
			DrawDebugLine
			(
				GetWorld(),
				PathPoints[PathPoints.Num() - 2], PathPoints[PathPoints.Num() - 1],
				FColor::Green,
				false,
				TimeStep * 2.0f,
				0,
				1.0f
			);
		}
	}
}

void AC_ThrowingWeapon::DrawPredictedPath()
{
	FPredictProjectilePathParams ProjectilePathParams{};
	ProjectilePathParams.StartLocation			= GetPredictedThrowStartLocation();
	ProjectilePathParams.LaunchVelocity			= ProjLaunchVelocity;
	ProjectilePathParams.bTraceWithCollision	= true;
	ProjectilePathParams.ProjectileRadius		= 5.f;
	ProjectilePathParams.MaxSimTime				= 1.f;
	ProjectilePathParams.bTraceWithChannel		= true;
	ProjectilePathParams.TraceChannel			= ECollisionChannel::ECC_Visibility;
	ProjectilePathParams.ObjectTypes			= {};
	ProjectilePathParams.ActorsToIgnore			= { this };
	ProjectilePathParams.SimFrequency			= 20.f;
	ProjectilePathParams.OverrideGravityZ		= ProjectileMovement->GetGravityZ();
	ProjectilePathParams.DrawDebugType			= EDrawDebugTrace::None;
	ProjectilePathParams.DrawDebugTime			= 1.f;
	ProjectilePathParams.bTraceComplex			= false;

	
	FPredictProjectilePathResult Result{};

	bool IsHit = UGameplayStatics::PredictProjectilePath(GetWorld(), ProjectilePathParams, Result);

	ClearSpline();

	TArray<FPredictProjectilePathPointData> PathData = Result.PathData;

	if (IsHit)
	{
		PredictedEndPoint->SetWorldLocation(PathData.Last().Location);
		PredictedEndPoint->SetVisibility(true);
	}

	for (auto& pointData : PathData) 
		PathSpline->AddSplinePoint(pointData.Location, ESplineCoordinateSpace::Local);

	PathSpline->SetSplinePointType(PathData.Num() - 1, ESplinePointType::CurveClamped);
	
	for (int i = 0; i < PathSpline->GetNumberOfSplinePoints() - 2; i++)
	{
		FVector FirstLocation{};
		FVector SecondLocation{};
		FVector FirstTangent{};
		FVector SecondTangent{};

		PathSpline->GetLocationAndTangentAtSplinePoint(i, FirstLocation, FirstTangent, ESplineCoordinateSpace::Local);
		PathSpline->GetLocationAndTangentAtSplinePoint(i + 1, SecondLocation, SecondTangent, ESplineCoordinateSpace::Local);

		USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this);
		SplineMeshComponent->SetStaticMesh(SplineMesh);
		SplineMeshComponent->SetStartAndEnd(FirstLocation, FirstTangent, SecondLocation, SecondTangent);
		SplineMeshComponent->SetMobility(EComponentMobility::Movable);
		SplineMeshComponent->RegisterComponentWithWorld(GetWorld());
		SplineMeshComponent->AttachToComponent(PathSpline, FAttachmentTransformRules::KeepRelativeTransform);
		//SplineMeshComponent->SetMaterial();
		
		SplineMeshes.Add(SplineMeshComponent);
	}
}

void AC_ThrowingWeapon::HandlePredictedPath()
{
	// �÷��̾��� ��쿡�� �׸��� (����, GameManager ��������� Player�� ��ü �����غ� ��)
	AC_Player* Player = Cast<AC_Player>(OwnerCharacter);
	if (!IsValid(Player)) return;

	// ���� OwnerCharacter�� �տ� ������ ��Ȳ���� Ȯ��
	if (!IsValid(this->GetAttachParentActor()) || this->GetAttachParentSocketName() != EQUIPPED_SOCKET_NAME) return;

	if (!OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(CurThrowProcessMontages.ThrowReadyMontage.AnimMontage))
		return;

	UpdateProjectileLaunchValues();
	DrawPredictedPath();
	//DrawDebugPredictedPath();
}

void AC_ThrowingWeapon::UpdateProjectileLaunchValues()
{
	// TODO : Init ProjStartLocation & ProjLaunchVelocity
	// TODO : Enemy AI�� ��� ����ź ������ ������ �ٸ� ������� ������� ��

	FRotator CameraRotation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraRotation();

	FVector Direction     = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::X).GetSafeNormal();	
	ProjStartLocation     = OwnerCharacter->GetMesh()->GetSocketLocation(THROW_START_SOCKET_NAME);
	ProjLaunchVelocity    = Direction * Speed;
	ProjLaunchVelocity.Z += UP_DIR_BOOST_OFFSET;
}

void AC_ThrowingWeapon::OnOwnerCharacterPoseTransitionFin()
{
	if (bIsCharging)
	{
		// ���ο� �ڼ��� �´� Anim Montage�� �ٽ� ���
		OwnerCharacter->PlayAnimMontage(CurThrowProcessMontages.ThrowReadyMontage);
		return;
	}


	//CurDrawMontage = DrawMontages[OwnerCharacter->GetPoseState()];

	//// ThrowProcessEnd ���� OnDrawEnd�� ����� ȣ���� �ȵǾ��� ���
	//// ���ǽ��� �Ȱɸ� ���� ����
	//if (
	//	OwnerCharacter->GetHandState() == EHandState::WEAPON_THROWABLE &&
	//	OwnerCharacter->GetEquippedComponent()->GetNextWeaponType() != EWeaponSlot::NONE //&&
	//	//!OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(CurDrawMontage.AnimMontage)
	//	)
	//{
	//	UC_Util::Print("Call OnDrawEnd Manually", FColor::Cyan, 5.f);
	//	OwnerCharacter->GetEquippedComponent()->OnDrawEnd();
	//}

}

void AC_ThrowingWeapon::ClearSpline()
{
	PredictedEndPoint->SetVisibility(false);

	for (auto& item : SplineMeshes) item->DestroyComponent();
	SplineMeshes.Empty();

	PathSpline->ClearSplinePoints();
}
