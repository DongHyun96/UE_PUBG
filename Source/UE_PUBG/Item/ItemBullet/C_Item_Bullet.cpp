// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemBullet/C_Item_Bullet.h"
#include "Item/Weapon/Gun/C_Gun.h"

#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"

#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_InvenSystem.h"
#include "Character/Component/C_EquippedComponent.h"

#include "HUD/C_HUDWidget.h"
#include "HUD/C_AmmoWidget.h"

#include "Utility/C_Util.h"

const TMap<EBulletType, FName> AC_Item_Bullet::BulletTypeNameMap =
{
	{EBulletType::FIVEMM,  "Item_Ammo_556mm_C"},
	{EBulletType::SEVENMM, "Item_Ammo_762mm_C"}
};

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
	if (ItemPlace == EItemPlace::AROUND)
	{
		return MoveAroundToInven(Character, this->GetItemCurStack());
	}
	else return false;
}

bool AC_Item_Bullet::MoveAroundToInven(AC_BasicCharacter* Character, int32 InStack)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : 안쓰는건 삭제하기.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : 안쓰는건 삭제하기.

	uint8 ItemStackCount = invenComp->LoopCheckVolume(this); //아이템Stack을 몇개까지 인벤에 넣을 수 있는가?

	if (ItemStackCount == 0)
	{
		UC_Util::Print("Not Enough Volume");
		return false; //인벤에 넣을 수 있는 아이템의 갯수가 0 이면 넣을 수 없으므로 return false;
	}

	if (ItemStackCount == this->GetItemCurStack())
	{
		//아이템을 전부 인벤에 넣을 수 있는 경우

		if (invenComp->FindMyItemByName(this->GetItemCode()))
		{
			// 인벤에 동일한 이름의 아이템이 존재 한다면 실행.

			invenComp->AddItemToMyList(this);
			UC_Util::Print(int(CurBulletType));
			UC_Util::Print("Item Stack Add!!!!!!!!!!!!!!!!");
			//this->Destroy(); // Inven에 존재하던 동일한 아이템과 합쳐졌으므로 삭제.
		}
		else
		{
			invenComp->AddItemToMyList(this);
		}
		if (AC_Player* OwnerPlayer = Cast<AC_Player>(Character))
			UpdateLeftAmmoWidget(OwnerPlayer); //Player만 실행.

		return true;
	}
	else
	{
		// 아이템을 일부만 넣을 수 있는 경우.
		this->SetItemStack(ItemCurStack - ItemStackCount);			//현재 객체의 stack을 조절
		AC_Item_Bullet* SpawnedItem = Cast<AC_Item_Bullet>(SpawnItem(Character));  	//동일한 아이템 객체를 생성
		SpawnedItem->SetItemStack(ItemStackCount);						 			//생성한 아이템 stack을 설정
																					
		if (invenComp->FindMyItemByName(this->GetItemCode()))
		{
			invenComp->AddItemToMyList(SpawnedItem);						 //inven에 추가
			//this->Destroy();
		}
		else
		{
			invenComp->AddItemToMyList(SpawnedItem);						 //inven에 추가
			UC_Util::Print(ItemStackCount);
		}

		if (AC_Player* OwnerPlayer = Cast<AC_Player>(Character))
			UpdateLeftAmmoWidget(OwnerPlayer); //Player만 실행.

		return true;
	}
}

bool AC_Item_Bullet::MoveInvenToAround(AC_BasicCharacter* Character, int32 InStack)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : 안쓰는건 삭제하기.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : 안쓰는건 삭제하기.

	if (!invenComp->FindMyItem(this)) return false;

	if (this->GetItemCurStack() > InStack)
	{
		AC_Item_Bullet* DroppedItem = Cast<AC_Item_Bullet>(SpawnItem(Character));
		this->SetItemStack(GetItemCurStack() - InStack);
		DroppedItem->SetItemStack(InStack);
		DroppedItem->DropItem(Character);
	}
	else
	{
		invenComp->RemoveItemToMyList(this);				 //내 아이템 리스트에서 아이템 제거.
		//invenComp->AddInvenCurVolume(-this->GetAllVolume()); // 버리는 아이템만큼 curVolume 조절하기. TODO : Inven에서 아이템 버릴 때 문제 생기면 체크하기.
		DropItem(Character);
	}
	//AddFivemmBulletStack을 통해서 총에 또 정보를 전달해 주어야 함
	if (AC_Player* OwnerPlayer = Cast<AC_Player>(Character))
		UpdateLeftAmmoWidget(OwnerPlayer); //Player만 실행

	return true;
}

void AC_Item_Bullet::UpdateLeftAmmoWidget(class AC_Player* InOwnerPlayer)
{
	if (InOwnerPlayer->GetHandState() == EHandState::WEAPON_GUN) //총을 들고 있다면 실행
	{
		AC_Gun* curGun = Cast<AC_Gun>(InOwnerPlayer->GetEquippedComponent()->GetCurWeapon());

		if (curGun->GetCurBulletType() == CurBulletType) //들고 있는 총이 사용하는 탄과 지금 습득한 이 총알과 Type이 같다면 실행.
		{
			AC_Item* curItem = InOwnerPlayer->GetInvenComponent()->FindMyItemByName(this->GetItemCode());
			int LeftAmmoStack = 0;
			if (curItem != nullptr)
			{
				LeftAmmoStack = curItem->GetItemCurStack();
			}
			//InOwnerPlayer->GetHUDWidget()->GetAmmoWidget()->SetLeftAmmoText(LeftAmmoStack);
		}
	}
}






