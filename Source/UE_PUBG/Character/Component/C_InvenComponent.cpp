// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Component/C_InvenComponent.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Character/Component/C_InvenSystem.h"
#include "Character/Component/C_EquippedComponent.h"

#include "Item/C_Item.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Equipment/C_BackPack.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "InvenUI/C_InventoryUIWidget.h"
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
		//PrevSlotEquipItem->DropItem(PrevSlotEquipItem->GetOwnerCharacter());
		if (PrevSlotEquipItem->GetItemDatas()->ItemType == EItemTypes::BACKPACK)
			MaxVolume -= CheckBackPackVolume(Cast<AC_BackPack>(PrevSlotEquipItem)->GetLevel());//TODO : MyBackPack을 GetEquipmentItems()[EEquipSlot::BACKPACK]로 대체하기.
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

	EquipmentItems[InSlot]->SetActorHiddenInGame(false);
	//RemoveItemToAroundList(EquipItem); // TODO : Collision을 키고 끄는 방식으로 할 지 아니면 강제로 넣고 빼줄지 생각

	return PrevSlotEquipItem;
}

AC_Item* UC_InvenComponent::FindMyItem(AC_Item* item)
{
	if (!item) return nullptr; // 유효성 검사

	if (TArray<AC_Item*>* ItemArrayPtr = MyItems.Find(item->GetItemCode()))
	{
		if (AC_Item* FoundItemPtr = ItemArrayPtr->Last()) 
		{
			return FoundItemPtr; 
		}
	}

	//TODO : CheckPoint - 장착, 사용 아이템의 위치가 어디에 있는지 확인할 것.
	return nullptr;
}

AC_Item* UC_InvenComponent::FindMyItemByName(const FName& itemName)
{
	if (TArray<AC_Item*>* ItemArrayPtr = MyItems.Find(itemName))
	{
		if (ItemArrayPtr->Num() > 0) // 배열이 비어 있지 않은지 확인
		{
			return ItemArrayPtr->Last(); // 마지막 원소 반환
		}
	}

	return nullptr;
}

bool UC_InvenComponent::HandleItemStackOverflow(AC_Item* InItem)
{
	if (!IsValid(InItem)) return false;

	// 해당 아이템의 코드로 배열을 가져옴 (여기서는 무조건 존재해야 함)
	TArray<AC_Item*>& ItemArray = MyItems[InItem->GetItemCode()];

	if (ItemArray.Num() == 0)
	{
		//UE_LOG(LogTemp, Warning, TEXT("HandleItemStackOverflow: 아이템 배열이 비어 있음! ItemCode: %s"), *item->GetItemCode().ToString());
		UC_Util::Print("ItemArray of HandleItemStackOverflow is NULL");
		return false;
	}

	AC_Item* LastItem = ItemArray.Last(); // 마지막 아이템 가져오기

	int32 MaxStack = LastItem->GetItemDatas()->ItemMaxStack;
	int32 NewStack = LastItem->GetItemCurStack() + InItem->GetItemCurStack();

	// 최대 스택 초과 여부 확인
	if (NewStack > MaxStack)
	{
		int32 OverflowAmount = NewStack - MaxStack;
		LastItem->SetItemStack(MaxStack); // 기존 아이템을 최대 스택까지만 설정

		// 초과된 개수를 새 아이템으로 생성 후 배열 끝에 추가
		AC_Item* NewItem = InItem->SpawnItem(OwnerCharacter);
		//NewItem = LastItem->SpawnItem(OwnerCharacter);
		NewItem->SetItemStack(OverflowAmount);
		NewItem->SetOwnerCharacter(OwnerCharacter);
		NewItem->SetItemPlace(EItemPlace::INVEN);
		NewItem->SetActorHiddenInGame(true);
		NewItem->SetActorEnableCollision(false);

		ItemArray.Add(NewItem);
	}
	else
	{
		// 초과하지 않으면 기존 아이템에 합산
		LastItem->SetItemStack(NewStack);
	}

	return true;
}

void UC_InvenComponent::AddItemToMyList(AC_Item* item)
{
	if (!IsValid(item)) return; // nullptr 방지

	// 아이템 코드로 MyItems에서 TArray<AC_Item*>를 찾음
	if (MyItems.Contains(item->GetItemCode()))
	{
		// HandleItemStackOverflow가 정상적으로 처리되었는지 확인
		if (!HandleItemStackOverflow(item))
		{
			UC_Util::Print("HandleItemStackOverflow Is False!");
			return;
		}

		CurVolume += item->GetItemAllVolume();

		// 원본 아이템 삭제 TODO : 혹시 중복 삭제가 되는 경우가 생기는지 확인하기.
		item->Destroy();
	}
	else
	{
		// 새로운 TArray 생성 후 아이템 추가
		MyItems.Add(item->GetItemCode(), { item });

		if (IsValid(OwnerCharacter))
		{
			item->SetOwnerCharacter(OwnerCharacter);
		}

		item->SetItemPlace(EItemPlace::INVEN);
		item->SetActorHiddenInGame(true);
		item->SetActorEnableCollision(false);

		CurVolume += item->GetItemAllVolume();

	}
	// UI 업데이트
	if (AC_Player* Player = Cast<AC_Player>(OwnerCharacter))
		Player->GetHUDWidget()->GetArmorInfoWidget()->SetCurrentBackPackCapacityRate(CurVolume / MaxVolume);
}

void UC_InvenComponent::RemoveItemToMyList(AC_Item* item)
{
	if (MyItems.Contains(item->GetItemCode()))
	{
		TArray<AC_Item*>& ItemArray = MyItems[item->GetItemCode()]; // 해당 아이템 코드의 아이템 배열 가져오기

		// 배열에서 아이템 찾기
		//int32 i = ItemArray->Num() - 1; i >= 0; i--
		for (int32 i = ItemArray.Num() - 1; i >= 0; --i)
		{
			if (ItemArray[i] == item) // 배열 내에서 일치하는 아이템 찾음
			{
				CurVolume -= item->GetItemAllVolume(); // 아이템 볼륨 감소
				ItemArray.RemoveAt(i); // 배열에서 아이템 제거

				// 배열이 비어 있으면 MyItems에서 해당 키 삭제
				if (ItemArray.Num() == 0)
				{
					MyItems.Remove(item->GetItemCode());
				}

				// 백팩 용량 갱신
				if (AC_Player* Player = Cast<AC_Player>(OwnerCharacter))
				{
					Player->GetHUDWidget()->GetArmorInfoWidget()->SetCurrentBackPackCapacityRate(CurVolume / MaxVolume);
				}
				return;
			}
		}
	}
}

void UC_InvenComponent::DestroyMyItem(AC_Item* DestroyedItem)
{
	RemoveItemToMyList(DestroyedItem);

	DestroyedItem->Destroy();

	//if (MyItems.Contains(DestroyedItem->GetItemCode())) // MyItems에 해당 아이템 코드가 존재하는지 확인
	//{
	//	TArray<AC_Item*>& ItemArray = MyItems[DestroyedItem->GetItemCode()]; // 해당 아이템 코드의 아이템 배열 가져오기
	//
	//	// 배열에서 아이템 찾기
	//	for (int32 i = 0; i < ItemArray.Num(); ++i)
	//	{
	//		if (ItemArray[i] == DestroyedItem) // 배열 내에서 일치하는 아이템 찾음
	//		{
	//			// 아이템 제거
	//			RemoveItemToMyList(ItemArray[i]);
	//			
	//			//if (ItemArray[i]) ItemArray.RemoveAt(i); // 배열에서 아이템 제거
	//			
	//			//ItemArray[i]->Destroy(); // 아이템 파괴
	//			break; // 해당 아이템을 찾았으므로 루프 종료
	//		}
	//	}
	//
	//	// 배열이 비어 있으면 MyItems에서 해당 키 삭제
	//	if (ItemArray.Num() == 0)
	//	{
	//		MyItems.Remove(DestroyedItem->GetItemCode());
	//	}
	//}
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


int32 UC_InvenComponent::GetTotalStackByItemName(const FName& ItemName)
{
	if (!MyItems.Contains(ItemName)) return 0; // 아이템이 없으면 0 반환

	int32 AllStack = 0;

	// 해당 아이템의 배열을 가져와서 순회하면서 CurStack을 합산
	for (AC_Item* Item : MyItems[ItemName])
	{
		if (IsValid(Item))
		{
			AllStack += Item->GetItemCurStack();
		}
	}

	return AllStack;
}

void UC_InvenComponent::DecreaseItemStack(const FName& ItemName, int32 Amount)
{
	TArray<AC_Item*>* ItemArray = MyItems.Find(ItemName);
	if (!ItemArray || ItemArray->Num() == 0) return;

	int32 RemainingAmount = Amount;

	// 뒤에서부터 차감
	for (int32 i = ItemArray->Num() - 1; i >= 0; i--)
	{
		AC_Item* Item = (*ItemArray)[i];
		if (!Item) continue;

		int32 CurrentStack = Item->GetItemCurStack(); // 현재 아이템 개수
		if (RemainingAmount >= CurrentStack)
		{
			// 현재 아이템을 전부 삭제해야 하는 경우
			RemainingAmount -= CurrentStack;
			//ItemArray->RemoveAt(i);
			Item->SetItemStack(0);
		}
		else
		{
			// 일부만 차감하는 경우
			Item->SetItemStack(CurrentStack - RemainingAmount);
			RemainingAmount = 0;
			break;
		}
	}
}




 

