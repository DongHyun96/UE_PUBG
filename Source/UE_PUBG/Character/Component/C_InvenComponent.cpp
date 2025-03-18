// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Component/C_InvenComponent.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Item/C_Item.h"
#include "Item/C_ItemBox.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Equipment/C_BackPack.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "Character/Component/C_EquippedComponent.h"
#include "HUD/C_ArmorInfoWidget.h"
#include "HUD/C_HUDWidget.h"

#include "Utility/C_Util.h"

// Sets default values for this component's properties
UC_InvenComponent::UC_InvenComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void UC_InvenComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
}


// Called every frame
void UC_InvenComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	
	// ...
}
/// <summary>
/// Item을 인벤에 넣을 때, 나의 최대 용량(MaxVolume)을 넘는지를 검사.
/// 아이템의 stack에 따른 용량의 변화 작업해야함.
/// </summary>
/// <param name="volume"></param>
/// <returns></returns>
bool UC_InvenComponent::CheckVolume(AC_Item* item)
{
	//item의 용량은 ItemVolume * ItemStack 이다.
	float ItemVolume = item->GetOnceVolume() * item->GetItemCurStack();

	if (MaxVolume > CurVolume + ItemVolume)
	{
		return true;
	}

	return false;
}
float UC_InvenComponent::LoopCheckVolume(AC_Item* item)
{
	for (float i = item->GetItemCurStack(); i > 0; i--)
	{
		if (MaxVolume > CurVolume + item->GetOnceVolume() * i) 
			return i;
	}
	return 0;
}

/// <summary>
/// 상호작용(F)함수 느낌으로 만들어봄.
/// wilditem은 map에 떨어져있는 item을 의미.
/// CheckVolume으로 내 인벤토리의 공간을 체크하고 아이템을 넣을 수 있는지 확인.
/// true면 맵에서 해당 object를 제거하고 Player가 소유하도록 하고자 한다.
/// AddActorComponentReplicatedSubObject 현재 이 함수가 어떤식으로 작용하고 결과를 주는지 잘 모르겠음.
/// 그렇다면 상호작용은 object에서 내용을 만들고 플레이어는 그 함수를 작동시키는 방식으로 가야 할 것 같음.
/// </summary>
/// <param name="wilditem"></param>
void UC_InvenComponent::Interaction(AC_Item* wilditem)
{
	//AC_BasicCharacter* player = OwnerCharacter;
	
	//wilditem->Interaction(OwnerCharacter);

	wilditem->Interaction(OwnerCharacter);


	if (CheckVolume(wilditem))
	{
		//상호작용된 아이템의 무게를 더해도 무게한도를 넘지 않는 경우.
		CurVolume += wilditem->GetOnceVolume();

		if (AC_Player* Player = Cast<AC_Player>(OwnerCharacter))
			Player->GetHUDWidget()->GetArmorInfoWidget()->SetCurrentBackPackCapacityRate(CurVolume / MaxVolume);
		
		//서버와 동기화하는데 주로 사용한다고함. 서버와 클라이언트간에 컴포넌트 상태를 동기화하고, 소유한다.
		wilditem->AddActorComponentReplicatedSubObject(this, wilditem);
	}
	else
	{
		//상호작용된 아이템의 무게를 더했을 때, 무게한도를 넘는 경우.

		//print("공간이 부족합니다"); 와 같은 멘트가 나오도록
		return;
	}
}
/// <summary>
/// 가방의 Level에 따라 알맞는 Volume(용량)을 반환해준다.
/// </summary>
/// <param name="backpacklevel"></param>
/// <returns></returns>
float UC_InvenComponent::CheckBackPackVolume(uint32 backpacklevel)
{
	switch (backpacklevel)
	{
	case 0:
		PreBackPackLevel = EBackPackLevel::LV0;
		return 50;
	case 1:
		PreBackPackLevel = EBackPackLevel::LV1;
		return 150;
	case 2:
		PreBackPackLevel = EBackPackLevel::LV2;
		return 200;
	case 3:
		PreBackPackLevel = EBackPackLevel::LV3;
		return 250;
	default:
		return 0;
		break;
	}
}

float UC_InvenComponent::CheckBackPackVolume(EBackPackLevel backpacklevel)
{
	switch (backpacklevel)
	{
	case EBackPackLevel::LV0:
		//PreBackPackLevel = EBackPackLevel::LV0;
		return 50;
	case EBackPackLevel::LV1:
		//PreBackPackLevel = EBackPackLevel::LV1;
		return 150;
	case EBackPackLevel::LV2:
		//PreBackPackLevel = EBackPackLevel::LV2;
		return 200;
	case EBackPackLevel::LV3:
		//PreBackPackLevel = EBackPackLevel::LV3;
		return 250;
	default:
		return 0;
		break;
	}
}

void UC_InvenComponent::DroppingItem(AC_Item* myitem)
{
	
}

void UC_InvenComponent::RemoveBackPack()
{
	if (!MyBackPack) return;

	MaxVolume -= CheckBackPackVolume(MyBackPack->GetLevel());

	MyBackPack = nullptr;

	CurBackPackLevel = EBackPackLevel::LV0;

	CheckBackPackOnCharacter();

}

AC_EquipableItem* UC_InvenComponent::SetSlotEquipment(EEquipSlot InSlot, AC_EquipableItem* EquipItem)
{
	AC_EquipableItem* PrevSlotEquipItem = EquipmentItems[InSlot];

	//기존의 장비가 있다면.
	if (PrevSlotEquipItem)
	{
		PrevSlotEquipItem->DetachItem(); //장착 해제.

		if (PrevSlotEquipItem->GetItemDatas()->ItemType == EItemTypes::BACKPACK)
			MaxVolume -= CheckBackPackVolume(Cast<AC_BackPack>(PrevSlotEquipItem)->GetLevel()); //TODO : MyBackPack을 GetEquipmentItems()[EEquipSlot::BACKPACK]로 대체하기.
		else if (PrevSlotEquipItem->GetItemDatas()->ItemType == EItemTypes::VEST)
			MaxVolume -= 50.f;
		//AddItemToAroundList(PrevSlotEquipItem);// TODO : Collision을 키고 끄는 방식으로 할 지 아니면 강제로 넣고 빼줄지 생각
	}
	EquipmentItems[InSlot] = EquipItem;
	
	if (EquipmentItems[InSlot] == nullptr)	return PrevSlotEquipItem; //nullptr이면 종료.

	if (EquipmentItems[InSlot]->GetItemDatas()->ItemType == EItemTypes::BACKPACK)
	{
		MyBackPack = Cast<AC_BackPack>(GetEquipmentItems()[EEquipSlot::BACKPACK]); //TODO : MyBackPack을 GetEquipmentItems()[EEquipSlot::BACKPACK]로 대체하기. 및 BackPack의 쓸데없이 Level이 2개임 하나로 통일하기.
		MaxVolume += CheckBackPackVolume(MyBackPack->GetLevel());
	}
	else if (EquipmentItems[InSlot]->GetItemDatas()->ItemType == EItemTypes::VEST)
		MaxVolume += 50.f;

	EquipmentItems[InSlot]->SetItemPlace(EItemPlace::SLOT);

	EquipmentItems[InSlot]->AttachToSocket(OwnerCharacter);

	EquipmentItems[InSlot]->SetActorEnableCollision(false);
	//RemoveItemToAroundList(EquipItem); // TODO : Collision을 키고 끄는 방식으로 할 지 아니면 강제로 넣고 빼줄지 생각

	return PrevSlotEquipItem;
}

AC_Item* UC_InvenComponent::FindMyItem(AC_Item* item)
{
	if (AC_Item** FoundItemPtr = MyItems.Find(item->GetItemCode()))
	{
		AC_Item* FoundItem = *FoundItemPtr;
		return FoundItem;
	}

	//TODO : CheckPoint - 장착, 사용 아이템의 위치가 어디에 있는지 확인할 것.
	return nullptr;
}

AC_Item* UC_InvenComponent::FindMyItemByName(FName itemName)
{
	if (AC_Item** FoundItemPtr = MyItems.Find(itemName))
	{
		AC_Item* FoundItem = *FoundItemPtr;
		return FoundItem;
	}

	return nullptr;
}

void UC_InvenComponent::AddItemToMyList(AC_Item* item)
{

	//AC_Item* FoundItem = FindMyItem(item); //인벤에 같은 아이템을 찾아옴, 없다면 nullptr;
	if (!IsValid(item)) return; //nullptr가 들어 오면 return.
	//if (testMyItems.Contains(item->GetItemDatas().ItemName))
	if (AC_Item** FoundItemPtr = MyItems.Find(item->GetItemCode()))
	{
		AC_Item* FoundItem = *FoundItemPtr;

		int32 sum = FoundItem->GetItemCurStack() + item->GetItemCurStack();

		FoundItem->SetItemStack(sum);

		CurVolume += item->GetAllVolume();

		if (AC_Player* Player = Cast<AC_Player>(OwnerCharacter))
			Player->GetHUDWidget()->GetArmorInfoWidget()->SetCurrentBackPackCapacityRate(CurVolume / MaxVolume);

		item->Destroy();//가지고 있던 아이템에 새로 넣는 아이템을 다 넣었으므로 삭제.
	}
	else 
	{
		// 해당키의 값이 추가.
		MyItems.Add(item->GetItemCode(), item);

		if (IsValid(OwnerCharacter))
		{
			item->SetOwnerCharacter(OwnerCharacter);
		}
		item->SetItemPlace(EItemPlace::INVEN);
		item->SetActorHiddenInGame(true);
		item->SetActorEnableCollision(false);

		CurVolume += item->GetAllVolume();

		if (AC_Player* Player = Cast<AC_Player>(OwnerCharacter))
			Player->GetHUDWidget()->GetArmorInfoWidget()->SetCurrentBackPackCapacityRate(CurVolume / MaxVolume);
	}
}

void UC_InvenComponent::RemoveItemToMyList(AC_Item* item)
{
	MyItems.Remove(item->GetItemCode());
	//Add와 달리 슬롯으로 가는 경우를 대비하여 OwnerCharacter의 설정을 건드리지 않았음.
	//item->SetItemPlace(EItemPlace::AROUND);
	CurVolume -= item->GetAllVolume();

	if (AC_Player* Player = Cast<AC_Player>(OwnerCharacter))
		Player->GetHUDWidget()->GetArmorInfoWidget()->SetCurrentBackPackCapacityRate(CurVolume / MaxVolume);
}

void UC_InvenComponent::DestroyMyItem(AC_Item* DestroyedItem)
{
	if (MyItems.Contains(DestroyedItem->GetItemCode()))
	{
		AC_Item* MyItem = MyItems[DestroyedItem->GetItemCode()];

		if (MyItem) //TMap에 해당 요소가 있는지 확인.
		{
			RemoveItemToMyList(MyItem);
			MyItem->Destroy();
		}
	}
}

void UC_InvenComponent::GetMapValues(const TMap<FString, AC_Item*>& Map, TArray<AC_Item*>& Values)
{
	Map.GenerateValueArray(Values);

}

void UC_InvenComponent::CheckBackPackOnCharacter()
{
	if (!OwnerCharacter) return;

	UC_EquippedComponent* equipComp = OwnerCharacter->GetEquippedComponent();
	AC_Gun* MainGunSlot = Cast<AC_Gun>(equipComp->GetWeapons()[EWeaponSlot::MAIN_GUN]);
	AC_Gun* SubGunSlot = Cast<AC_Gun>(equipComp->GetWeapons()[EWeaponSlot::SUB_GUN]);

	//equipComp->GetWeapons().Find(EWeaponSlot::MAIN_GUN)


	if (MainGunSlot)
		MainGunSlot->CheckBackPackLevelChange();

	if (SubGunSlot)
		SubGunSlot->CheckBackPackLevelChange();
}

void UC_InvenComponent::AddInvenCurVolume(float ItemVolume)
{
	CurVolume += ItemVolume;

	if (AC_Player* Player = Cast<AC_Player>(OwnerCharacter))
		Player->GetHUDWidget()->GetArmorInfoWidget()->SetCurrentBackPackCapacityRate(CurVolume / MaxVolume);
}

float UC_InvenComponent::GetVestVolume()
{
	if (!EquipmentItems[EEquipSlot::VEST]) return 0.0f;
	return 50.0f;
}


void UC_InvenComponent::InitMyitems()
{

}




 

