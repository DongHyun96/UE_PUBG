// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Component/C_InvenComponent.h"
#include "Character/C_BasicCharacter.h"
#include "Item/C_Item.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Equipment/C_BackPack.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "Character/Component/C_EquippedComponent.h"

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
	float ItemVolume = item->GetOnceVolume() * item->GetItemDatas().ItemCurStack;

	if (MaxVolume > CurVolume + ItemVolume)
	{
		return true;
	}

	return false;
}
float UC_InvenComponent::LoopCheckVolume(AC_Item* item)
{
	for (float i = item->GetItemDatas().ItemCurStack; i > 0; i--)
	{
		if (MaxVolume > CurVolume + item->GetOnceVolume() * i) 
			return i;
	}
	return 0;
}
/// <summary>
/// 1. Holster가 nullptr이면 maxVolume+= volume을 해준다.
/// Holster가 ture면 가방의 레벨을 비교. 
/// 2. 바꾸려는 가방의 레벨이 높으면 기존가방의 vulume을 새로운 가방의 volume에서 뺀만큼만 character의 maxVulume을 더해준다. maxVolume += (새가방의 volume - 기존가방의 volume);을 해준다.
/// 3. 바꾸려는 가방의 레벨이 낮으면 testVolume = 새가방의 volume - 기존가방의 volume;
/// if (maxVolume - testVolume > curVolume)
/// {
///		//가방을 바꾸어줌.
/// }
/// else
/// {
///		//가방이 바뀌지 않음.
/// }
/// backpack이 nullptr일 때(가방을 버리고자 할 때) CurBackPacklevel을 0으로 만든다.
/// bool 반환함수로 만들어서 가방을 바꿀 수 있을때 true를 아니라면 false를 반환하도록 하도록 하였음.
/// </summary>
bool UC_InvenComponent::CheckMyBackPack(AC_BackPack* backpack)
{
	//
	//backpack->Destroy
	if (backpack == nullptr)
	{
		CurBackPackLevel = EBackPackLevel::LV0;
		EquippedBackPack(backpack);
		return true;
	}

	CheckBackPackVolume(backpack->GetLevel());

	if (CurBackPackLevel < PreBackPackLevel)
	{
		//현재가방보다 바꾸려는 가방이 레벨이 높을때.
		FString TheStr0 = TEXT("Upgrade Backpack");
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Black, TheStr0);

		MaxVolume += CheckBackPackVolume(backpack->GetLevel());

		if (CurBackPackLevel != EBackPackLevel::LV0)
		{
			MaxVolume -= CheckBackPackVolume(CurBackPackLevel);
		}

		CurBackPackLevel = PreBackPackLevel;

		MyBackPack->DetachToSocket(this->OwnerCharacter);

		MyBackPack = backpack;

		//가방도 내 아이템 리스트에 포함하는 함수. 현재는 고려중.
		//MyItem.Add(backpack);
		//backpack->Destroy();
		//backpack->AttachToSocket(OwnerCharacter);
		EquippedBackPack(backpack);
		return true;
	}
	else if (CurBackPackLevel > PreBackPackLevel)
	{
		//다음 용량 = 현재가방상태의 최대 용량 - (현재 가방의 용량 - 다음 가방의 용량)
		float NextVolume = MaxVolume - (CheckBackPackVolume(CurBackPackLevel) - CheckBackPackVolume(backpack->GetLevel()));
		
		//
		if (CurVolume < NextVolume)
		{
			FString TheStr1 = TEXT("success to downgrade");
			GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Black, TheStr1);

			//현재 용량보다 다음 용량이 크다면 배낭을 변경.
			MaxVolume = NextVolume;
			CurBackPackLevel = PreBackPackLevel;
			MyBackPack->DetachToSocket(this->OwnerCharacter);
			EquippedBackPack(backpack);
			return true;
		}
		else if (CurVolume > NextVolume)
		{
			FString TheStr2 = TEXT("failed to downgrade");
			GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Black, TheStr2);
			//현재 용량이 다음 용량보다 크다면 배낭 변경이 불가능.
			//해당 상황에 대한 문구 출력.

			return false;
		}
		else
		{
			FString TheStr3 = TEXT("success to change backpack");
			GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Black, TheStr3);

			//현재 용량과 다음 용량이 같으므로 가방의 변경이 가능.
			MyBackPack->DetachToSocket(this->OwnerCharacter);
			EquippedBackPack(backpack);
			return true;
		}
	}
	else
	{
		FString TheStr4 = TEXT("same backpack level");
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Black, TheStr4);
		
		//현재 가방과 다음 가방의 레벨이 같으면 가방 변경 가능
		UC_Util::Print((float)MaxVolume);
		MyBackPack->DetachToSocket(this->OwnerCharacter);
		EquippedBackPack(backpack);
		return true;
	}
}
/// <summary>
/// Bag 아이템에서 상호작용(interaction)에서 모두 처리할까?
/// Bag에서 Character의 BagLevel을 변화 시키고자한다.
/// 이때 Bag의 Interactertion에서 
/// </summary>


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

void UC_InvenComponent::EquippedBackPack(AC_BackPack* backpack)
{
	if (MyBackPack) RemoveBackPack();

	backpack->AttachToSocket(OwnerCharacter);
	MyBackPack = backpack;

	MaxVolume += CheckBackPackVolume(MyBackPack->GetLevel());

	CurBackPackLevel = PreBackPackLevel;

	//원래 장착하면 OverlapEnd로 인해 자동으로 없어져야 한다고 생각하는데 안사라져서 강제로 지웠음.
	NearItems.Remove(backpack);

	CheckBackPackOnCharacter();
}

bool UC_InvenComponent::AddItem(AC_Item* item)
{
	if (!item) return false;

	if (CheckVolume(item))
	{
		CurVolume += item->GetOnceVolume();

		//이부분도 아이템에서 처리
		//item->SetOwnerCharacter(OwnerCharacter);
		//item->SetActorHiddenInGame(true);
		//item->SetActorEnableCollision(false);
		FString AddedItemName;
		EItemTypes type = item->GetItemDatas().ItemType;
		switch (type)
		{
		case EItemTypes::NONE:
			break;
		case EItemTypes::HELMET:
		case EItemTypes::VEST:
		case EItemTypes::BACKPACK:
		case EItemTypes::MAINGUN:
		case EItemTypes::MELEEWEAPON:
		case EItemTypes::ATTACHMENT:
			item->PickUpItem(OwnerCharacter);
			break;
		case EItemTypes::THROWABLE:
		case EItemTypes::CONSUMPTIONITEM:
			//AddedItemName = TEXT("NONE");
			AddedItemName = item->GetItemDatas().ItemName;
			if (testMyItems.Contains(AddedItemName))
			{
				//현재 파밍하려는 아이템이 이미 인벤(MyItemList)에 존재 하는 경우.
				// InPutStack = 현재 아이템의 스택 + 파밍한 아이템의 스택
				//uint8 InPutStack = testMyItems[AddedItemName]->GetItemDatas().ItemStack + item->GetItemDatas().ItemStack;
				uint8 InPutStack = testMyItems[AddedItemName].Last()->GetItemDatas().ItemCurStack + item->GetItemDatas().ItemCurStack;

				testMyItems[AddedItemName].Last()->SetItemStack(InPutStack);

				//testMyItems.Remove(AddedItemName);
				item->SetActorHiddenInGame(true);
				item->SetActorEnableCollision(false);
				//가지고 있던 아이템을 업데이트 했으므로 가져오던 아이템은 삭제.
				item->Destroy();
			}
			else
			{
				//현재 파밍하려는 아이템이 인벤(MyItemList)에 없는 아이템 인 경우.
				item->PickUpItem(OwnerCharacter);
				if (TArray<AC_Item*>* FoundArray = testMyItems.Find(AddedItemName)) 
				{
					FoundArray->Add(item);
				}
				else 
				{
					// 키가 없으면 새로운 배열을 생성하고 추가
					TArray<AC_Item*> NewArray;
					NewArray.Add(item);
					testMyItems.Add(AddedItemName, NewArray);
				}
				//testMyItems.Add(AddedItemName, item);
			}
			break;
		default:
			//AddedItemName = TEXT("NONE");
			break;
		}
		//아이템을 습득할 때 아이템의 종류에 따라 알아서 습득, 장착등이 되도록.


		///////////////////


		return true;
	}
	return false;
}

AC_EquipableItem* UC_InvenComponent::SetSlotEquipment(EEquipSlot InSlot, AC_EquipableItem* EquipItem)
{
	AC_EquipableItem* PrevSlotEquipItem = EquipmentItems[InSlot];

	//기존의 장비가 있다면.
	if (PrevSlotEquipItem)
	{
		PrevSlotEquipItem->DetachItem(); //장착 해제.

		if (PrevSlotEquipItem->GetItemDatas().ItemType == EItemTypes::BACKPACK)
			MaxVolume -= CheckBackPackVolume(Cast<AC_BackPack>(PrevSlotEquipItem)->GetLevel()); //TODO : MyBackPack을 GetEquipmentItems()[EEquipSlot::BACKPACK]로 대체하기.
		//AddItemToAroundList(PrevSlotEquipItem);// TODO : Collision을 키고 끄는 방식으로 할 지 아니면 강제로 넣고 빼줄지 생각
	}
	EquipmentItems[InSlot] = Cast<AC_EquipableItem>(EquipItem);
	
	if (EquipmentItems[InSlot] == nullptr)	return PrevSlotEquipItem; //nullptr이면 종료.

	if (EquipmentItems[InSlot]->GetItemDatas().ItemType == EItemTypes::BACKPACK)
	{
		MyBackPack = Cast<AC_BackPack>(GetEquipmentItems()[EEquipSlot::BACKPACK]); //TODO : MyBackPack을 GetEquipmentItems()[EEquipSlot::BACKPACK]로 대체하기. 및 BackPack의 쓸데없이 Level이 2개임 하나로 통일하기.
		MaxVolume += CheckBackPackVolume(MyBackPack->GetLevel());
	}
	EquipmentItems[InSlot]->SetItemPlace(EItemPlace::SLOT);

	EquipmentItems[InSlot]->AttachToSocket(OwnerCharacter);

	EquipmentItems[InSlot]->SetActorEnableCollision(false);
	//RemoveItemToAroundList(EquipItem); // TODO : Collision을 키고 끄는 방식으로 할 지 아니면 강제로 넣고 빼줄지 생각

	return PrevSlotEquipItem;
}

AC_Item* UC_InvenComponent::FindMyItem(AC_Item* item)
{
	AC_Item* FoundItem = nullptr;
	if (testMyItems.Find(item->GetItemDatas().ItemName))
		FoundItem = testMyItems.Find(item->GetItemDatas().ItemName)->Last();

	//TODO : CheckPoint - 장착, 사용 아이템의 위치가 어디에 있는지 확인할 것.

	return FoundItem;
}

AC_Item* UC_InvenComponent::FindMyItem(FString itemName)
{
	AC_Item* FoundItem = nullptr;
	if (testMyItems.Find(itemName))
		FoundItem = testMyItems.Find(itemName)->Last();

	return FoundItem;
}

void UC_InvenComponent::AddItemToMyList(AC_Item* item)
{
	//AC_Item* FoundItem = FindMyItem(item); //인벤에 같은 아이템을 찾아옴, 없다면 nullptr;
	if (!IsValid(item)) return; //nullptr가 들어 오면 return.
	if (TArray<AC_Item*>* FoundArray = testMyItems.Find(item->GetItemDatas().ItemName))
	{
		//item과 같은 이름의 아이템이 있다면 TArray에 추가?
		//if (item->GetItemDatas().ItemCurStack < item->GetItemDatas().ItemMaxStack)
		//TODO : 부착물과 meleeweapon에 대한 처리.
		int sum = FoundArray->Last()->GetItemDatas().ItemCurStack + item->GetItemDatas().ItemCurStack;
		if (sum <= MAX_STACK)
		{
			FoundArray->Last()->SetItemStack(sum);
			item->Destroy();//가지고 있던 아이템에 새로 넣는 아이템을 다 넣었으므로 삭제.
		}
		else
		{
			FoundArray->Last()->SetItemStack(MAX_STACK);
			item->SetItemStack(MAX_STACK - FoundArray->Last()->GetItemDatas().ItemCurStack);
			FoundArray->Emplace(item);
		}
	}
	else 
	{
		// 해당키의 값이 없으면 새로운 배열을 생성하고 추가
		TArray<AC_Item*> NewArray;
		NewArray.Add(item);
		testMyItems.Add(item->GetItemDatas().ItemName, NewArray);
	}
	//testMyItems.Add(item->GetItemDatas().ItemName, item);

	if (IsValid(OwnerCharacter))
	{
		item->SetOwnerCharacter(OwnerCharacter);
	}
	item->SetItemPlace(EItemPlace::INVEN);
	item->SetActorHiddenInGame(true);
	item->SetActorEnableCollision(false);

	CurVolume += item->GetAllVolume();
}

void UC_InvenComponent::RemoveItemToMyList(AC_Item* item)
{
	testMyItems.Remove(item->GetItemDatas().ItemName);
	//Add와 달리 슬롯으로 가는 경우를 대비하여 OwnerCharacter의 설정을 건드리지 않았음.
	//item->SetItemPlace(EItemPlace::AROUND);
	CurVolume -= item->GetAllVolume();
}

void UC_InvenComponent::DestroyMyItem(AC_Item* DestroyedItem)
{
	if (testMyItems.Contains(DestroyedItem->GetItemDatas().ItemName))
	{
		TArray<AC_Item*>& ItemArray = testMyItems[DestroyedItem->GetItemDatas().ItemName];

		if (ItemArray.Num() > 0) // 배열에 요소가 있는지 확인
		{
			// 마지막 요소 가져오기
			AC_Item* LastItem = ItemArray.Last();

			if (LastItem)
			{
				// 안전하게 Destroy 호출
				LastItem->Destroy();
			}

			// 배열에서 마지막 요소 제거
			ItemArray.RemoveAt(ItemArray.Num() - 1);
		}
		else
		{

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
}


void UC_InvenComponent::InitMyitems()
{

}




 