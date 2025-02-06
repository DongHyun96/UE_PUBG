// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemBullet/C_Item_Bullet.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"

#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_InvenSystem.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenSystem.h"	
#include "Utility/C_Util.h"
AC_Item_Bullet::AC_Item_Bullet()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AC_Item_Bullet::BeginPlay()
{
	Super::BeginPlay();
}

void AC_Item_Bullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AC_Item_Bullet::Interaction(AC_BasicCharacter* Character)
{
	if (ItemDatas.ItemPlace == EItemPlace::AROUND)
	{
		return MoveAroundToInven(Character);
	}
	else return false;
}

bool AC_Item_Bullet::MoveAroundToInven(AC_BasicCharacter* Character)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : 안쓰는건 삭제하기.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : 안쓰는건 삭제하기.

	uint8 ItemStackCount = invenComp->LoopCheckVolume(this); //아이템Stack을 몇개까지 인벤에 넣을 수 있는가?

	if (ItemStackCount == 0)
	{
		UC_Util::Print("Not Enough Volume");
		return false; //인벤에 넣을 수 있는 아이템의 갯수가 0 이면 넣을 수 없으므로 return false;
	}

	if (ItemStackCount == this->GetItemDatas().ItemCurStack)
	{
		//아이템을 전부 인벤에 넣을 수 있는 경우.
		invenComp->AddItemToMyList(this);
		AddBulletStackToCharacter();
		return true;
	}
	else
	{
		//아이템을 전부 넣을 수 없는 경우.
		this->SetItemStack(GetItemDatas().ItemCurStack - ItemStackCount);//현재 객체의 stack을 조절
		AC_Item_Bullet* SpawnedItem = Cast<AC_Item_Bullet>(SpawnItem(Character));  //동일한 아이템 객체를 생성
		SpawnedItem->SetItemStack(ItemStackCount);						 //생성한 아이템 stack을 설정
		invenComp->AddItemToMyList(SpawnedItem);						 //inven에 추가.
		AddBulletStackToCharacter();
		return true;
	}
}

bool AC_Item_Bullet::MoveInvenToAround(AC_BasicCharacter* Character)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : 안쓰는건 삭제하기.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : 안쓰는건 삭제하기.

	if (!invenComp->FindMyItem(this)) return false;

	invenComp->RemoveItemToMyList(this);				 //내 아이템 리스트에서 아이템 제거.

	//invenComp->AddInvenCurVolume(-this->GetAllVolume()); //버리는 아이템만큼 curVolume 조절하기. TODO : Inven에서 아이템 버릴 때 문제 생기면 체크하기.

	DropItem(Character);

	return true;
}




void AC_Item_Bullet::AddBulletStackToCharacter()
{
	if (!OwnerCharacter) return;
	
	switch (CurBulletType)
	{
	case EBulletType::FIVEMM:
		OwnerCharacter->AddFivemmBulletStack(ItemDatas.ItemCurStack);
		break;
	case EBulletType::SEVENMM:
		OwnerCharacter->AddSevenmmBulletStack(ItemDatas.ItemCurStack);

		break;
	case EBulletType::NONE:
		break;
	default:
		break;
	}
}


