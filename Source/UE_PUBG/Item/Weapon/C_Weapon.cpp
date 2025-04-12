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

//void AC_Weapon::PickUpItem(AC_BasicCharacter* Character)
//{
//	EItemTypes Type = ItemDataRef->ItemType;
//
//	switch (Type)
//	{
//	case EItemTypes::MAINGUN:
//		PickUpItem(Character);
//		break;
//	case EItemTypes::MELEEWEAPON:
//		PickUpItem(Character);
//		break;
//	case EItemTypes::THROWABLE:
//		PickUpItem(Character);
//		break;
//	default:
//		break;
//	}
//}
//
//bool AC_Weapon::Interaction(AC_BasicCharacter* Character)
//{
//	if (Character)
//	{
//		PickUpItem(Character);
//		return true;
//	}
//	return false;
//}

bool AC_Weapon::MoveSlotToAround(AC_BasicCharacter* Character, int32 InStack)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : 안쓰는건 삭제하기.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : 안쓰는건 삭제하기.

	//Slot에 있다는건 장착된 상태라는 것. Around로 간다는건 장착을 해제하고 아이템을 땅에 떨군다는 뜻.
	EWeaponSlot curWeaponSlot = GetWeaponSlot();

	AC_Weapon* curWeapon = equipComp->GetWeapons()[curWeaponSlot];

	//if (curWeapon != this) return false; //장착된 아이템이 자신이 아니라면 return.
	
	//if (curWeaponSlot == EWeaponSlot::MAIN_GUN)
	//	curWeapon->DetachmentItem();
	//else
	equipComp->SetSlotWeapon(curWeaponSlot, nullptr); //장착된 아이템이 자신이면, 장착해제를 진행.

	DropItem(Character);

	return true;
}

bool AC_Weapon::MoveSlotToInven(AC_BasicCharacter* Character, int32 InStack)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : 안쓰는건 삭제하기.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : 안쓰는건 삭제하기.
	//if (invenComp->GetMaxVolume() > invenComp->GetCurVolume() + GetOnceVolume())
	if (!invenComp->CheckVolume(this)) return false; //인벤에 this가 들어가서 curVolume > MaxVolume이 된다면 return.

	EWeaponSlot curWeaponSlot = GetWeaponSlot();

	AC_Weapon* curWeapon = equipComp->GetWeapons()[curWeaponSlot];

	if (curWeapon != this) return false; //장착된 아이템이 자신이 아니라면 return.

	equipComp->SetSlotWeapon(curWeaponSlot, nullptr); //장착된 아이템이 자신이면, 장착해제를 진행.

	invenComp->AddItemToMyList(this);//장착 해제된 아이템을 내 아이템 리스트에 추가.

	return true;
}

//Gun만 사용 O
bool AC_Weapon::MoveSlotToSlot(AC_BasicCharacter* Character, int32 InStack)
{
	return false;
}

//Gun만 사용 X
bool AC_Weapon::MoveInvenToAround(AC_BasicCharacter* Character, int32 InStack)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : 안쓰는건 삭제하기.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : 안쓰는건 삭제하기.

	if (!invenComp->FindMyItem(this)) return false;

	invenComp->RemoveItemToMyList(this);				 //내 아이템 리스트에서 아이템 제거.

	//invenComp->AddInvenCurVolume(-this->GetAllVolume()); //버리는 아이템만큼 curVolume 조절하기. TODO : Inven에서 아이템 버릴 때 문제 생기면 체크하기.

	DropItem(Character);

	return true; 
}

//사용하지 않음.
bool AC_Weapon::MoveInvenToInven(AC_BasicCharacter* Character, int32 InStack)
{
	return false;
}

//Gun만 사용 X
bool AC_Weapon::MoveInvenToSlot(AC_BasicCharacter* Character, int32 InStack)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : 안쓰는건 삭제하기.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : 안쓰는건 삭제하기.

	EWeaponSlot curWeaponSlot = GetWeaponSlot();

	AC_Weapon* curWeapon = equipComp->GetWeapons()[curWeaponSlot];

	if (curWeapon)
	{
		float prevVolume = invenComp->GetCurVolume() + curWeapon->GetOnceVolume() - this->GetOnceVolume();
		if (prevVolume > invenComp->GetMaxVolume()) return false; //교체하는 아이템이 인벤에 들어오면서 MaxVolume을 넘으면 return.
	}

	if (ItemCurStack == 1)//MeleeWeapon의 갯수는 0으로 해놨음;; TODO : 새로운 방법 찾기, 찾지 못한다면 우선 MeleeWeapon의 stack을 1로 사용하기.
	{
		curWeapon = equipComp->SetSlotWeapon(curWeaponSlot, this);
		invenComp->RemoveItemToMyList(this);
	}
	else
	{
		this->DeductItemStack();
		AC_ThrowingWeapon* SwapItem = Cast<AC_ThrowingWeapon>(SpawnItem(Character));
		SwapItem->SetItemStack(1);
		curWeapon = equipComp->SetSlotWeapon(curWeaponSlot, SwapItem);
	}
	invenComp->AddItemToMyList(curWeapon);//내부에서 매개변수 nullptr가 들어오면 return시켜버림. TODO : curWeapon을 정의한 뒤에 equipComp->GetWeapons()[EWeaponSlot::THROWABLE_WEAPON]의 값이 바뀌었으므로 역참조를 하면 문제가 생김. 확인 할 것.
	
	return true;
}

//사용하지 않음.
bool AC_Weapon::MoveAroundToAround(AC_BasicCharacter* Character, int32 InStack)
{
	return false;
}

//Gun만 사용 X
bool AC_Weapon::MoveAroundToInven(AC_BasicCharacter* Character, int32 InStack)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : 안쓰는건 삭제하기.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : 안쓰는건 삭제하기.

	uint8 ItemStackCount = invenComp->LoopCheckVolume(this); //아이템Stack을 몇개까지 인벤에 넣을 수 있는가?

	if (ItemStackCount == 0)
	{
		UC_Util::Print("Not Enough Volume");
		return false; //인벤에 넣을 수 있는 아이템의 갯수가 0 이면 넣을 수 없으므로 return false;
	}

	if (ItemStackCount == ItemCurStack)
	{
		//아이템을 전부 인벤에 넣을 수 있는 경우.
		invenComp->AddItemToMyList(this);
		return true;
	}
	else
	{
		//아이템을 일부만 넣을 수 있는 경우.
		this->SetItemStack(ItemCurStack - ItemStackCount);               //현재 객체의 stack을 조절
		AC_Weapon* SpawnedItem = Cast<AC_Weapon>(SpawnItem(Character));  //동일한 아이템 객체를 생성
		SpawnedItem->SetItemStack(ItemStackCount);						 //생성한 아이템 stack을 설정
		invenComp->AddItemToMyList(SpawnedItem);						 //inven에 추가.
		return true;
	}
}

bool AC_Weapon::MoveAroundToSlot(AC_BasicCharacter* Character, int32 InStack)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : 안쓰는건 삭제하기.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : 안쓰는건 삭제하기.

	EWeaponSlot curWeaponSlot = GetWeaponSlot();

	AC_Weapon* curWeapon = equipComp->GetWeapons()[curWeaponSlot];

	if (curWeapon)
	{
		if (!curWeapon->MoveSlotToInven(Character, InStack)) //이곳에서 curWeapon의 장착을 해제함.
			curWeapon->MoveSlotToAround(Character,InStack);
	}

	if (ItemCurStack == 1) //Gun과 MeleeWeapon은 stack = 0 임. TODO : 새로운 방법 찾기, 우선은 curStack = 1로 사용. 
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

bool AC_Weapon::ExecuteAIAttackTickTask(class AC_BasicCharacter* InTargetCharacter, const float& DeltaTime)
{
	return false;
}

EWeaponSlot AC_Weapon::GetWeaponSlot()
{
	switch (this->GetItemDatas()->ItemType)
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


