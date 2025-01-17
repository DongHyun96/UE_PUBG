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
/// Item�� �κ��� ���� ��, ���� �ִ� �뷮(MaxVolume)�� �Ѵ����� �˻�.
/// �������� stack�� ���� �뷮�� ��ȭ �۾��ؾ���.
/// </summary>
/// <param name="volume"></param>
/// <returns></returns>
bool UC_InvenComponent::CheckVolume(AC_Item* item)
{
	//item�� �뷮�� ItemVolume * ItemStack �̴�.
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
/// 1. Holster�� nullptr�̸� maxVolume+= volume�� ���ش�.
/// Holster�� ture�� ������ ������ ��. 
/// 2. �ٲٷ��� ������ ������ ������ ���������� vulume�� ���ο� ������ volume���� ����ŭ�� character�� maxVulume�� �����ش�. maxVolume += (�������� volume - ���������� volume);�� ���ش�.
/// 3. �ٲٷ��� ������ ������ ������ testVolume = �������� volume - ���������� volume;
/// if (maxVolume - testVolume > curVolume)
/// {
///		//������ �ٲپ���.
/// }
/// else
/// {
///		//������ �ٲ��� ����.
/// }
/// backpack�� nullptr�� ��(������ �������� �� ��) CurBackPacklevel�� 0���� �����.
/// bool ��ȯ�Լ��� ���� ������ �ٲ� �� ������ true�� �ƴ϶�� false�� ��ȯ�ϵ��� �ϵ��� �Ͽ���.
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
		//���簡�溸�� �ٲٷ��� ������ ������ ������.
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

		//���浵 �� ������ ����Ʈ�� �����ϴ� �Լ�. ����� �����.
		//MyItem.Add(backpack);
		//backpack->Destroy();
		//backpack->AttachToSocket(OwnerCharacter);
		EquippedBackPack(backpack);
		return true;
	}
	else if (CurBackPackLevel > PreBackPackLevel)
	{
		//���� �뷮 = ���簡������� �ִ� �뷮 - (���� ������ �뷮 - ���� ������ �뷮)
		float NextVolume = MaxVolume - (CheckBackPackVolume(CurBackPackLevel) - CheckBackPackVolume(backpack->GetLevel()));
		
		//
		if (CurVolume < NextVolume)
		{
			FString TheStr1 = TEXT("success to downgrade");
			GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Black, TheStr1);

			//���� �뷮���� ���� �뷮�� ũ�ٸ� �賶�� ����.
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
			//���� �뷮�� ���� �뷮���� ũ�ٸ� �賶 ������ �Ұ���.
			//�ش� ��Ȳ�� ���� ���� ���.

			return false;
		}
		else
		{
			FString TheStr3 = TEXT("success to change backpack");
			GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Black, TheStr3);

			//���� �뷮�� ���� �뷮�� �����Ƿ� ������ ������ ����.
			MyBackPack->DetachToSocket(this->OwnerCharacter);
			EquippedBackPack(backpack);
			return true;
		}
	}
	else
	{
		FString TheStr4 = TEXT("same backpack level");
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Black, TheStr4);
		
		//���� ����� ���� ������ ������ ������ ���� ���� ����
		UC_Util::Print((float)MaxVolume);
		MyBackPack->DetachToSocket(this->OwnerCharacter);
		EquippedBackPack(backpack);
		return true;
	}
}
/// <summary>
/// Bag �����ۿ��� ��ȣ�ۿ�(interaction)���� ��� ó���ұ�?
/// Bag���� Character�� BagLevel�� ��ȭ ��Ű�����Ѵ�.
/// �̶� Bag�� Interactertion���� 
/// </summary>


/// <summary>
/// ��ȣ�ۿ�(F)�Լ� �������� ����.
/// wilditem�� map�� �������ִ� item�� �ǹ�.
/// CheckVolume���� �� �κ��丮�� ������ üũ�ϰ� �������� ���� �� �ִ��� Ȯ��.
/// true�� �ʿ��� �ش� object�� �����ϰ� Player�� �����ϵ��� �ϰ��� �Ѵ�.
/// AddActorComponentReplicatedSubObject ���� �� �Լ��� ������� �ۿ��ϰ� ����� �ִ��� �� �𸣰���.
/// �׷��ٸ� ��ȣ�ۿ��� object���� ������ ����� �÷��̾�� �� �Լ��� �۵���Ű�� ������� ���� �� �� ����.
/// </summary>
/// <param name="wilditem"></param>
void UC_InvenComponent::Interaction(AC_Item* wilditem)
{
	//AC_BasicCharacter* player = OwnerCharacter;
	
	//wilditem->Interaction(OwnerCharacter);

	wilditem->Interaction(OwnerCharacter);


	if (CheckVolume(wilditem))
	{
		//��ȣ�ۿ�� �������� ���Ը� ���ص� �����ѵ��� ���� �ʴ� ���.
		CurVolume += wilditem->GetOnceVolume();
		//������ ����ȭ�ϴµ� �ַ� ����Ѵٰ���. ������ Ŭ���̾�Ʈ���� ������Ʈ ���¸� ����ȭ�ϰ�, �����Ѵ�.
		wilditem->AddActorComponentReplicatedSubObject(this, wilditem);
	}
	else
	{
		//��ȣ�ۿ�� �������� ���Ը� ������ ��, �����ѵ��� �Ѵ� ���.

		//print("������ �����մϴ�"); �� ���� ��Ʈ�� ��������
		return;
	}
}
/// <summary>
/// ������ Level�� ���� �˸´� Volume(�뷮)�� ��ȯ���ش�.
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

	//���� �����ϸ� OverlapEnd�� ���� �ڵ����� �������� �Ѵٰ� �����ϴµ� �Ȼ������ ������ ������.
	NearItems.Remove(backpack);

	CheckBackPackOnCharacter();
}

bool UC_InvenComponent::AddItem(AC_Item* item)
{
	if (!item) return false;

	if (CheckVolume(item))
	{
		CurVolume += item->GetOnceVolume();

		//�̺κе� �����ۿ��� ó��
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
				//���� �Ĺ��Ϸ��� �������� �̹� �κ�(MyItemList)�� ���� �ϴ� ���.
				// InPutStack = ���� �������� ���� + �Ĺ��� �������� ����
				//uint8 InPutStack = testMyItems[AddedItemName]->GetItemDatas().ItemStack + item->GetItemDatas().ItemStack;
				uint8 InPutStack = testMyItems[AddedItemName].Last()->GetItemDatas().ItemCurStack + item->GetItemDatas().ItemCurStack;

				testMyItems[AddedItemName].Last()->SetItemStack(InPutStack);

				//testMyItems.Remove(AddedItemName);
				item->SetActorHiddenInGame(true);
				item->SetActorEnableCollision(false);
				//������ �ִ� �������� ������Ʈ �����Ƿ� �������� �������� ����.
				item->Destroy();
			}
			else
			{
				//���� �Ĺ��Ϸ��� �������� �κ�(MyItemList)�� ���� ������ �� ���.
				item->PickUpItem(OwnerCharacter);
				if (TArray<AC_Item*>* FoundArray = testMyItems.Find(AddedItemName)) 
				{
					FoundArray->Add(item);
				}
				else 
				{
					// Ű�� ������ ���ο� �迭�� �����ϰ� �߰�
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
		//�������� ������ �� �������� ������ ���� �˾Ƽ� ����, �������� �ǵ���.


		///////////////////


		return true;
	}
	return false;
}

AC_EquipableItem* UC_InvenComponent::SetSlotEquipment(EEquipSlot InSlot, AC_EquipableItem* EquipItem)
{
	AC_EquipableItem* PrevSlotEquipItem = EquipmentItems[InSlot];

	//������ ��� �ִٸ�.
	if (PrevSlotEquipItem)
	{
		PrevSlotEquipItem->DetachItem(); //���� ����.

		if (PrevSlotEquipItem->GetItemDatas().ItemType == EItemTypes::BACKPACK)
			MaxVolume -= CheckBackPackVolume(Cast<AC_BackPack>(PrevSlotEquipItem)->GetLevel()); //TODO : MyBackPack�� GetEquipmentItems()[EEquipSlot::BACKPACK]�� ��ü�ϱ�.
		//AddItemToAroundList(PrevSlotEquipItem);// TODO : Collision�� Ű�� ���� ������� �� �� �ƴϸ� ������ �ְ� ������ ����
	}
	EquipmentItems[InSlot] = Cast<AC_EquipableItem>(EquipItem);
	
	if (EquipmentItems[InSlot] == nullptr)	return PrevSlotEquipItem; //nullptr�̸� ����.

	if (EquipmentItems[InSlot]->GetItemDatas().ItemType == EItemTypes::BACKPACK)
	{
		MyBackPack = Cast<AC_BackPack>(GetEquipmentItems()[EEquipSlot::BACKPACK]); //TODO : MyBackPack�� GetEquipmentItems()[EEquipSlot::BACKPACK]�� ��ü�ϱ�. �� BackPack�� �������� Level�� 2���� �ϳ��� �����ϱ�.
		MaxVolume += CheckBackPackVolume(MyBackPack->GetLevel());
	}
	EquipmentItems[InSlot]->SetItemPlace(EItemPlace::SLOT);

	EquipmentItems[InSlot]->AttachToSocket(OwnerCharacter);

	EquipmentItems[InSlot]->SetActorEnableCollision(false);
	//RemoveItemToAroundList(EquipItem); // TODO : Collision�� Ű�� ���� ������� �� �� �ƴϸ� ������ �ְ� ������ ����

	return PrevSlotEquipItem;
}

AC_Item* UC_InvenComponent::FindMyItem(AC_Item* item)
{
	AC_Item* FoundItem = nullptr;
	if (testMyItems.Find(item->GetItemDatas().ItemName))
		FoundItem = testMyItems.Find(item->GetItemDatas().ItemName)->Last();

	//TODO : CheckPoint - ����, ��� �������� ��ġ�� ��� �ִ��� Ȯ���� ��.

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
	//AC_Item* FoundItem = FindMyItem(item); //�κ��� ���� �������� ã�ƿ�, ���ٸ� nullptr;
	if (!IsValid(item)) return; //nullptr�� ��� ���� return.
	if (TArray<AC_Item*>* FoundArray = testMyItems.Find(item->GetItemDatas().ItemName))
	{
		//item�� ���� �̸��� �������� �ִٸ� TArray�� �߰�?
		//if (item->GetItemDatas().ItemCurStack < item->GetItemDatas().ItemMaxStack)
		//TODO : �������� meleeweapon�� ���� ó��.
		int sum = FoundArray->Last()->GetItemDatas().ItemCurStack + item->GetItemDatas().ItemCurStack;
		if (sum <= MAX_STACK)
		{
			FoundArray->Last()->SetItemStack(sum);
			item->Destroy();//������ �ִ� �����ۿ� ���� �ִ� �������� �� �־����Ƿ� ����.
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
		// �ش�Ű�� ���� ������ ���ο� �迭�� �����ϰ� �߰�
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
	//Add�� �޸� �������� ���� ��츦 ����Ͽ� OwnerCharacter�� ������ �ǵ帮�� �ʾ���.
	//item->SetItemPlace(EItemPlace::AROUND);
	CurVolume -= item->GetAllVolume();
}

void UC_InvenComponent::DestroyMyItem(AC_Item* DestroyedItem)
{
	if (testMyItems.Contains(DestroyedItem->GetItemDatas().ItemName))
	{
		TArray<AC_Item*>& ItemArray = testMyItems[DestroyedItem->GetItemDatas().ItemName];

		if (ItemArray.Num() > 0) // �迭�� ��Ұ� �ִ��� Ȯ��
		{
			// ������ ��� ��������
			AC_Item* LastItem = ItemArray.Last();

			if (LastItem)
			{
				// �����ϰ� Destroy ȣ��
				LastItem->Destroy();
			}

			// �迭���� ������ ��� ����
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




 