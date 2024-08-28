// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Component/C_InvenComponent.h"
#include "Character/C_BasicCharacter.h"
#include "Item/C_Item.h"
#include "Item/Equipment/C_BackPack.h"
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
/// </summary>
/// <param name="volume"></param>
/// <returns></returns>
bool UC_InvenComponent::CheckVolume(uint16 volume)
{
	if (MaxVolume > CurVolume + volume)
	{
		return true;
	}

	return false;
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
bool UC_InvenComponent::ChackMyBackPack(AC_BackPack* backpack)
{
	//
	//backpack->Destroy
	if (backpack == nullptr)
	{
		CurBackPackLevel = EBackPackLevel::LV0;
		backpack->AttachToSocket(OwnerCharacter->GetMesh());
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
		MyBackPack = backpack;

		//���浵 �� ������ ����Ʈ�� �����ϴ� �Լ�. ����� �����.
		//MyItem.Add(backpack);
		backpack->AttachToSocket(OwnerCharacter->GetMesh());

		return true;
	}
	else if (CurBackPackLevel > PreBackPackLevel)
	{
		//���� �뷮 = ���簡������� �ִ� �뷮 - (���� ������ �뷮 - ���� ������ �뷮)
		uint16 NextVolume = MaxVolume - (CheckBackPackVolume(CurBackPackLevel) - CheckBackPackVolume(backpack->GetLevel()));
		
		//
		if (CurVolume < NextVolume)
		{
			FString TheStr1 = TEXT("success to downgrade");
			GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Black, TheStr1);

			//���� �뷮���� ���� �뷮�� ũ�ٸ� �賶�� ����.
			MaxVolume = NextVolume;
			CurBackPackLevel = PreBackPackLevel;
			MyBackPack = backpack;
			backpack->AttachToSocket(OwnerCharacter->GetMesh());
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
			MyBackPack = backpack;
			backpack->AttachToSocket(OwnerCharacter->GetMesh());
			return true;
		}
	}
	else
	{
		FString TheStr4 = TEXT("same backpack level");
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Black, TheStr4);
		
		UC_Util::Print((float)MaxVolume);

		//���� ����� ���� ������ ������ ������ ���� ���� ����
		MyBackPack = backpack;
		backpack->AttachToSocket(OwnerCharacter->GetMesh());
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
	
	wilditem->Interaction(OwnerCharacter);

	if (CheckVolume(wilditem->GetVolume()))
	{
		//��ȣ�ۿ�� �������� ���Ը� ���ص� �����ѵ��� ���� �ʴ� ���.
		CurVolume += wilditem->GetVolume();
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
uint16 UC_InvenComponent::CheckBackPackVolume(uint32 backpacklevel)
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

uint16 UC_InvenComponent::CheckBackPackVolume(EBackPackLevel backpacklevel)
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
	MaxVolume -= CheckBackPackVolume(MyBackPack->GetLevel());

	MyBackPack = nullptr;

	CurBackPackLevel = EBackPackLevel::LV0;
}




 