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

		if (invenComp->FindMyItem(this->GetItemDatas().ItemName))
		{
			//�κ��� ������ �̸��� �������� ���� �Ѵٸ� ����.

			invenComp->AddItemToMyList(this);
			AddBulletStackToCharacter();

			if (AC_Player* OwnerPlayer = Cast<AC_Player>(Character))
				UpdateLeftAmmoWidget(OwnerPlayer); //Player�� ����.

			this->Destroy(); //Inven�� �����ϴ� ������ �����۰� ���������Ƿ� ����.
		}
		else
		{
			invenComp->AddItemToMyList(this);
			AddBulletStackToCharacter();

			if (AC_Player* OwnerPlayer = Cast<AC_Player>(Character))
				UpdateLeftAmmoWidget(OwnerPlayer); //Player�� ����.
		}
		

		return true;
	}
	else
	{
		//�������� �Ϻθ� ���� �� �ִ� ���.
		this->SetItemStack(GetItemDatas().ItemCurStack - ItemStackCount);//���� ��ü�� stack�� ����
		AC_Item_Bullet* SpawnedItem = Cast<AC_Item_Bullet>(SpawnItem(Character));  //������ ������ ��ü�� ����
		SpawnedItem->SetItemStack(ItemStackCount);						 //������ ������ stack�� ����

		if (invenComp->FindMyItem(this->GetItemDatas().ItemName))
		{
			invenComp->AddItemToMyList(SpawnedItem);						 //inven�� �߰�.

			AddBulletStackToCharacter();

			if (AC_Player* OwnerPlayer = Cast<AC_Player>(Character))
				UpdateLeftAmmoWidget(OwnerPlayer); //Player�� ����.

			this->Destroy();
		}
		else
		{
			invenComp->AddItemToMyList(SpawnedItem);						 //inven�� �߰�.

			AddBulletStackToCharacter();

			if (AC_Player* OwnerPlayer = Cast<AC_Player>(Character))
				UpdateLeftAmmoWidget(OwnerPlayer); //Player�� ����.
		}

		return true;
	}
}

bool AC_Item_Bullet::MoveInvenToAround(AC_BasicCharacter* Character)
{
	UC_EquippedComponent* equipComp = Character->GetEquippedComponent();//TODO : �Ⱦ��°� �����ϱ�.
	UC_InvenComponent* invenComp = Character->GetInvenComponent();		//TODO : �Ⱦ��°� �����ϱ�.

	if (!invenComp->FindMyItem(this)) return false;

	invenComp->RemoveItemToMyList(this);				 //�� ������ ����Ʈ���� ������ ����.

	DeBulletStackToCharacter();

	//invenComp->AddInvenCurVolume(-this->GetAllVolume()); //������ �����۸�ŭ curVolume �����ϱ�. TODO : Inven���� ������ ���� �� ���� ����� üũ�ϱ�.

	DropItem(Character);
	//AddFivemmBulletStack�� ���ؼ� �ѿ� �� ������ ���� �� �־�� ��.
	if (AC_Player* OwnerPlayer = Cast<AC_Player>(Character))
		UpdateLeftAmmoWidget(OwnerPlayer); //Player�� ����.

	return true;
}

void AC_Item_Bullet::UpdateLeftAmmoWidget(class AC_Player* InOwnerPlayer)
{
	if (InOwnerPlayer->GetHandState() == EHandState::WEAPON_GUN) //���� ��� �ִٸ� ����
	{
		AC_Gun* curGun = Cast<AC_Gun>(InOwnerPlayer->GetEquippedComponent()->GetCurWeapon());

		if (curGun->GetCurBulletType() == CurBulletType) //��� �ִ� ���� ����ϴ� ź�� ���� ������ �� �Ѿ˰� Type�� ���ٸ� ����.
		{
			AC_Item* curItem = InOwnerPlayer->GetInvenComponent()->FindMyItem(this->GetItemDatas().ItemName);
			int LeftAmmoStack = 0;
			if (curItem != nullptr)
			{
				LeftAmmoStack = curItem->GetItemDatas().ItemCurStack;
			}
			InOwnerPlayer->GetHUDWidget()->GetAmmoWidget()->SetLeftAmmoText(LeftAmmoStack);
		}
	}
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

void AC_Item_Bullet::DeBulletStackToCharacter()
{
	if (!OwnerCharacter) return;

	switch (CurBulletType)
	{
	case EBulletType::FIVEMM:
		OwnerCharacter->AddFivemmBulletStack(-ItemDatas.ItemCurStack);
		break;
	case EBulletType::SEVENMM:
		OwnerCharacter->AddSevenmmBulletStack(-ItemDatas.ItemCurStack);

		break;
	case EBulletType::NONE:
		break;
	default:
		break;
	}
}
