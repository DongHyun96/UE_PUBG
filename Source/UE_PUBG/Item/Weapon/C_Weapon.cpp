// Fill out your copyright notice in the Description page of Project Settings.



#include "Item/Weapon/C_Weapon.h"
#include "UE_PUBG/Item/C_Item.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/C_BasicCharacter.h"
#include "Gun/C_Gun.h"
#include "MeleeWeapon/C_MeleeWeapon.h"
#include "ThrowingWeapon/C_ThrowingWeapon.h"
#include "Utility/C_Util.h"
#include "Item/Weapon/WeaponStrategy/I_WeaponButtonStrategy.h"


// Sets default values
AC_Weapon::AC_Weapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AC_Weapon::BeginPlay()
{
	Super::BeginPlay();

	InitialRelativeTransform = RootComponent->GetRelativeTransform();
}

// Called every frame
void AC_Weapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

bool AC_Weapon::ExecuteBKey()
{
	if (!WeaponButtonStrategy)
	{
		FString DebugMessage = "In AC_Weapon::ExecuteBKey : WeaponStrategy nullptr!";
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *DebugMessage);

		return false;
	}
	return WeaponButtonStrategy->UseBKeyStrategy(OwnerCharacter, this);
}

bool AC_Weapon::ExecuteRKey()
{
	if (!WeaponButtonStrategy)
	{
		FString DebugMessage = "In AC_Weapon::ExecuteRKey : WeaponStrategy nullptr!";
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *DebugMessage);

		return false;
	}
	return WeaponButtonStrategy->UseRKeyStrategy(OwnerCharacter, this);
}

bool AC_Weapon::ExecuteMlb_Started()
{
	if (!WeaponButtonStrategy)
	{
		FString DebugMessage = "In AC_Weapon::ExecuteMlb_Started : WeaponStrategy nullptr!";
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *DebugMessage);

		return false;
	}
	return WeaponButtonStrategy->UseMlb_StartedStrategy(OwnerCharacter, this);
}

bool AC_Weapon::ExecuteMlb_OnGoing()
{
	if (!WeaponButtonStrategy) return false;
	return WeaponButtonStrategy->UseMlb_OnGoingStrategy(OwnerCharacter, this);
}

bool AC_Weapon::ExecuteMlb_Completed()
{
	if (!WeaponButtonStrategy) return false;
	return WeaponButtonStrategy->UseMlb_CompletedStrategy(OwnerCharacter, this);
}

bool AC_Weapon::ExecuteMrb_Started()
{
	if (!WeaponButtonStrategy)
	{
		FString DebugMessage = "In AC_Weapon::ExecuteMlb_Started : WeaponStrategy nullptr!";
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *DebugMessage);

		return false;
	}
	return WeaponButtonStrategy->UseMrb_StartedStrategy(OwnerCharacter, this);
}

bool AC_Weapon::ExecuteMrb_OnGoing()
{
	if (!WeaponButtonStrategy) return false;
	return WeaponButtonStrategy->UseMrb_OnGoingStrategy(OwnerCharacter, this);
}

bool AC_Weapon::ExecuteMrb_Completed()
{
	if (!WeaponButtonStrategy) return false;
	return WeaponButtonStrategy->UseMrb_CompletedStrategy(OwnerCharacter, this);
}

void AC_Weapon::PickUpItem(AC_BasicCharacter* Character)
{
	EItemTypes Type = ItemDatas.ItemType;

	switch (Type)
	{
	case EItemTypes::MAINGUN:
		PickUpItem(Character);
		break;
	case EItemTypes::MELEEWEAPON:
		PickUpItem(Character);
		break;
	case EItemTypes::THROWABLE:
		PickUpItem(Character);
		break;
	default:
		break;
	}
}

bool AC_Weapon::Interaction(AC_BasicCharacter* Character)
{
	if (Character)
	{
		PickUpItem(Character);
		return true;
	}
	return false;
}

bool AC_Weapon::MoveSlotToAround(AC_BasicCharacter* Character)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : �Ⱦ��°� �����ϱ�.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : �Ⱦ��°� �����ϱ�.

	//Slot�� �ִٴ°� ������ ���¶�� ��. Around�� ���ٴ°� ������ �����ϰ� �������� ���� �����ٴ� ��.
	EWeaponSlot curWeaponSlot = GetWeaponSlot();

	AC_Weapon* curWeapon = equipComp->GetWeapons()[curWeaponSlot];

	if (curWeapon != this) return false; //������ �������� �ڽ��� �ƴ϶�� return.
	
	//if (curWeaponSlot == EWeaponSlot::MAIN_GUN)
	//	curWeapon->DetachmentItem();
	//else
	DropItem(Character);

	equipComp->SetSlotWeapon(curWeaponSlot, nullptr); //������ �������� �ڽ��̸�, ���������� ����.

	return true;
}

bool AC_Weapon::MoveSlotToInven(AC_BasicCharacter* Character)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : �Ⱦ��°� �����ϱ�.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : �Ⱦ��°� �����ϱ�.
	//if (invenComp->GetMaxVolume() > invenComp->GetCurVolume() + GetOnceVolume())
	if (!invenComp->CheckVolume(this)) return false; //�κ��� this�� ���� curVolume > MaxVolume�� �ȴٸ� return.

	EWeaponSlot curWeaponSlot = GetWeaponSlot();

	AC_Weapon* curWeapon = equipComp->GetWeapons()[curWeaponSlot];

	if (curWeapon != this) return false; //������ �������� �ڽ��� �ƴ϶�� return.

	equipComp->SetSlotWeapon(curWeaponSlot, nullptr); //������ �������� �ڽ��̸�, ���������� ����.

	invenComp->AddItemToMyList(this);//���� ������ �������� �� ������ ����Ʈ�� �߰�.

	return true;
}

//Gun�� ��� O
bool AC_Weapon::MoveSlotToSlot(AC_BasicCharacter* Character)
{
	return false;
}

//Gun�� ��� X
bool AC_Weapon::MoveInvenToAround(AC_BasicCharacter* Character)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : �Ⱦ��°� �����ϱ�.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : �Ⱦ��°� �����ϱ�.

	if (!invenComp->FindMyItem(this)) return false;

	invenComp->RemoveItemToMyList(this);				 //�� ������ ����Ʈ���� ������ ����.

	//invenComp->AddInvenCurVolume(-this->GetAllVolume()); //������ �����۸�ŭ curVolume �����ϱ�. TODO : Inven���� ������ ���� �� ���� ����� üũ�ϱ�.

	DropItem(Character);

	return true; 
}

//������� ����.
bool AC_Weapon::MoveInvenToInven(AC_BasicCharacter* Character)
{
	return false;
}

//Gun�� ��� X
bool AC_Weapon::MoveInvenToSlot(AC_BasicCharacter* Character)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : �Ⱦ��°� �����ϱ�.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : �Ⱦ��°� �����ϱ�.

	EWeaponSlot curWeaponSlot = GetWeaponSlot();

	AC_Weapon* curWeapon = equipComp->GetWeapons()[curWeaponSlot];

	if (curWeapon)
	{
		float prevVolume = invenComp->GetCurVolume() + curWeapon->GetOnceVolume() - this->GetOnceVolume();
		if (prevVolume > invenComp->GetMaxVolume()) return false; //��ü�ϴ� �������� �κ��� �����鼭 MaxVolume�� ������ return.
	}

	if (this->GetItemDatas().ItemCurStack == 1)//MeleeWeapon�� ������ 0���� �س���;; TODO : ���ο� ��� ã��, ã�� ���Ѵٸ� �켱 MeleeWeapon�� stack�� 1�� ����ϱ�.
	{
		equipComp->SetSlotWeapon(curWeaponSlot, this);
		invenComp->RemoveItemToMyList(this);
	}
	else
	{
		this->DeductItemStack();
		AC_ThrowingWeapon* SwapItem = Cast<AC_ThrowingWeapon>(SpawnItem(Character));
		SwapItem->SetItemStack(1);
		equipComp->SetSlotWeapon(curWeaponSlot, SwapItem);
	}
	invenComp->AddItemToMyList(curWeapon);//���ο��� �Ű����� nullptr�� ������ return���ѹ���. TODO : curWeapon�� ������ �ڿ� equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON]�� ���� �ٲ�����Ƿ� �������� �ϸ� ������ ����. Ȯ�� �� ��.
	
	return true;
}

//������� ����.
bool AC_Weapon::MoveAroundToAround(AC_BasicCharacter* Character)
{
	return false;
}

//Gun�� ��� X
bool AC_Weapon::MoveAroundToInven(AC_BasicCharacter* Character)
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
		//�������� �Ϻθ� ���� �� �ִ� ���.
		this->SetItemStack(GetItemDatas().ItemCurStack - ItemStackCount);//���� ��ü�� stack�� ����
		AC_Weapon* SpawnedItem = Cast<AC_Weapon>(SpawnItem(Character));  //������ ������ ��ü�� ����
		SpawnedItem->SetItemStack(ItemStackCount);						 //������ ������ stack�� ����
		invenComp->AddItemToMyList(SpawnedItem);						 //inven�� �߰�.
		return true;
	}
}

bool AC_Weapon::MoveAroundToSlot(AC_BasicCharacter* Character)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : �Ⱦ��°� �����ϱ�.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : �Ⱦ��°� �����ϱ�.

	EWeaponSlot curWeaponSlot = GetWeaponSlot();

	AC_Weapon* curWeapon = equipComp->GetWeapons()[curWeaponSlot];

	if (curWeapon)
	{
		if (!curWeapon->MoveSlotToInven(Character)) //�̰����� curWeapon�� ������ ������.
			curWeapon->MoveSlotToAround(Character);
	}

	if (this->GetItemDatas().ItemCurStack == 1) //Gun�� MeleeWeapon�� stack = 0 ��. TODO : ���ο� ��� ã��, �켱�� curStack = 1�� ���. 
	{
		equipComp->SetSlotWeapon(curWeaponSlot, this);
		invenComp->RemoveItemToAroundList(this);
	}
	else
	{
		this->DeductItemStack();
		AC_Weapon* SwapItem = Cast<AC_Weapon>(SpawnItem(Character));
		SwapItem->SetItemStack(1);
		equipComp->SetSlotWeapon(curWeaponSlot, SwapItem);
	}
	return true;
}

EWeaponSlot AC_Weapon::GetWeaponSlot()
{
	switch (this->GetItemDatas().ItemType)
	{
	case EItemTypes::MAINGUN:

		if (Cast<AC_Gun>(this)->GetCurrentWeaponState() == EGunState::SUB_GUN)
			return EWeaponSlot::SUB_GUN;

		return EWeaponSlot::MAIN_GUN;

	case EItemTypes::MELEEWEAPON:
		return EWeaponSlot::MELEE_WEAPON;

	case EItemTypes::THROWABLE:
		return EWeaponSlot::THROWABLE_WEAPON;

	default:
		return EWeaponSlot::NONE;
	}
}
