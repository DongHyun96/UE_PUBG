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
/// Item을 인벤에 넣을 때, 나의 최대 용량(MaxVolume)을 넘는지를 검사.
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
		//현재가방보다 바꾸려는 가방이 레벨이 높을때.
		FString TheStr0 = TEXT("Upgrade Backpack");
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Black, TheStr0);

		MaxVolume += CheckBackPackVolume(backpack->GetLevel());

		if (CurBackPackLevel != EBackPackLevel::LV0)
		{
			MaxVolume -= CheckBackPackVolume(CurBackPackLevel);
		}

		CurBackPackLevel = PreBackPackLevel;
		MyBackPack = backpack;

		//가방도 내 아이템 리스트에 포함하는 함수. 현재는 고려중.
		//MyItem.Add(backpack);
		backpack->AttachToSocket(OwnerCharacter->GetMesh());

		return true;
	}
	else if (CurBackPackLevel > PreBackPackLevel)
	{
		//다음 용량 = 현재가방상태의 최대 용량 - (현재 가방의 용량 - 다음 가방의 용량)
		uint16 NextVolume = MaxVolume - (CheckBackPackVolume(CurBackPackLevel) - CheckBackPackVolume(backpack->GetLevel()));
		
		//
		if (CurVolume < NextVolume)
		{
			FString TheStr1 = TEXT("success to downgrade");
			GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Black, TheStr1);

			//현재 용량보다 다음 용량이 크다면 배낭을 변경.
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
			//현재 용량이 다음 용량보다 크다면 배낭 변경이 불가능.
			//해당 상황에 대한 문구 출력.

			return false;
		}
		else
		{
			FString TheStr3 = TEXT("success to change backpack");
			GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Black, TheStr3);

			//현재 용량과 다음 용량이 같으므로 가방의 변경이 가능.
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

		//현재 가방과 다음 가방의 레벨이 같으면 가방 변경 가능
		MyBackPack = backpack;
		backpack->AttachToSocket(OwnerCharacter->GetMesh());
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
	
	wilditem->Interaction(OwnerCharacter);

	if (CheckVolume(wilditem->GetVolume()))
	{
		//상호작용된 아이템의 무게를 더해도 무게한도를 넘지 않는 경우.
		CurVolume += wilditem->GetVolume();
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




 