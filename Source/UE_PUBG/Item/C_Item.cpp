// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/C_Item.h"
#include "Utility/C_Util.h"
#include "Character/C_BasicCharacter.h"

#include "Engine/World.h"

#include "Weapon/WeaponStrategy/I_WeaponButtonStrategy.h"
#include "Weapon/WeaponStrategy/C_GunStrategy.h"

// Sets default values
AC_Item::AC_Item()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    //WeaponButtonStrategy = CreateDefaultSubobject<AC_GunStrategy>("GunStrategy");

	//OwnerCharacter = nullptr;
}

// Called when the game starts or when spawned
void AC_Item::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AC_Item::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_Item::DetachmentItem()
{
	
	if (!OwnerCharacter) return;
	
	DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);

	//�Ŀ� ���� Ʈ���̽��� ����ؼ� �ٲ��ֱ�.
	SetActorLocation(OwnerCharacter->GetActorLocation() + FVector(0.f, 0.f, -75.f));
	SetActorRotation(FRotator(0.f, 0.f, -90.f));
	SetOwnerCharacter(nullptr);

	//�ѹ� ���� ����� OverlapBegin�� �۵� -> �����Ҷ� ���ְ� ���� �� ���ָ� �ɵ�?
	SetActorEnableCollision(false);
	SetActorEnableCollision(true);
	
	UC_Util::Print("Check");
}

void AttachToSocket(USceneComponent* InParent)
{

}

//AC_Item* SpawnItem(AC_Item* OriginalItem, const AC_BasicCharacter* Character)
//{
//	FActorSpawnParameters SpawnParams;
//
//	AC_Item* SpawnedItme = Character->GetWorld()->SpawnActor<AC_Item>(OriginalItem->GetClass(), Character->GetActorLocation(), Character->GetActorRotation(), SpawnParams);
//
//
//	return nullptr;
//}

FVector AC_Item::GetGroundLocation(AC_BasicCharacter* Character)
{
	// ĳ������ ��ġ
	FVector CharacterLocation = Character->GetActorLocation();

	// ���� Ʈ���̽� ���� �� �� ��ġ ����
	FVector TraceStart = CharacterLocation;
	FVector TraceEnd = TraceStart - FVector(0.0f, 0.0f, 10000.0f); // �Ʒ� �������� 10,000 ����

	// ���� Ʈ���̽� ����
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character); // ĳ���ʹ� ����

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_Visibility, // �浹 ä��
		Params
	);

	if (bHit)
	{
		UC_Util::Print("Hit Ground");
		UC_Util::Print(HitResult.GetActor()->GetName());

	}
	else
	{
		UC_Util::Print("Not Hit Ground");
	}

	return HitResult.Location;
}

AC_Item* AC_Item::SpawnItem(AC_BasicCharacter* Character)
	{
	FActorSpawnParameters SpawnParams;
	//SpawnParams.Owner = Character;
	//location, rotation�� this�� ���� ���� �͵� ����, �ֳ��ϸ� ���� �̻��ϰ� ���󰡴� ������ �̰��� �� �� ����. -> ����ź�� ������ �浹ü�� ��������.
	AC_Item* SpawnItem = GetWorld()->SpawnActor<AC_Item>(this->GetClass(), GetGroundLocation(Character) + RootComponent->Bounds.BoxExtent.Z, Character->GetActorRotation(), SpawnParams);
	//SpawnItem->SetItemStack(1);
	//SpawnItem->SetActorHiddenInGame(true);
	SpawnItem->SetActorEnableCollision(false);//������ �� ������ OverlapBegine�� �����ؼ� �켱 ������.
	return SpawnItem;
}

bool AC_Item::MoveToInven(AC_BasicCharacter* Character)
{
	
	switch (GetItemDatas().ItemPlace)
	{
	case EItemPlace::AROUND:
		MoveAroundToInven(Character);
		break;
	case EItemPlace::INVEN:
		MoveInvenToInven(Character);
		break;
	case EItemPlace::SLOT:
		MoveSlotToInven(Character);
		break;
	default:
		break;
	}
	return false;
}

bool AC_Item::MoveToAround(AC_BasicCharacter* Character)
{
	switch (GetItemDatas().ItemPlace)
	{
	case EItemPlace::AROUND:
		MoveAroundToAround(Character);
		break;
	case EItemPlace::INVEN:
		MoveInvenToAround(Character);
		break;
	case EItemPlace::SLOT:
		MoveSlotToAround(Character);
		break;
	default:
		break;
	}
	return false;
}

bool AC_Item::MoveToSlot(AC_BasicCharacter* Character)
{
	switch (GetItemDatas().ItemPlace)
	{
	case EItemPlace::AROUND:
		MoveAroundToSlot(Character);
		break;
	case EItemPlace::INVEN:
		MoveInvenToSlot(Character);
		break;
	case EItemPlace::SLOT:
		MoveSlotToSlot(Character);
		break;
	default:
		break;
	}
	return false;
}

bool AC_Item::MoveSlotToInven(AC_BasicCharacter* Character)
{
	return false;
}

bool AC_Item::MoveSlotToSlot(AC_BasicCharacter* Character)
{
	return false;
}

bool AC_Item::MoveInvenToAround(AC_BasicCharacter* Character)
{
	return false;
}

bool AC_Item::MoveInvenToInven(AC_BasicCharacter* Character)
{
	return false;
}			  
bool AC_Item::MoveInvenToSlot(AC_BasicCharacter* Character)
{
	return false;
}

bool AC_Item::MoveAroundToAround(AC_BasicCharacter* Character)
{
	return false;
}

bool AC_Item::MoveAroundToInven(AC_BasicCharacter* Character)
{
	return false;
}

bool AC_Item::MoveAroundToSlot(AC_BasicCharacter* Character)
{
	return false;
}

void AC_Item::DropItem(AC_BasicCharacter* Character)
{
	//TODO : �������� ����(Attach)�Ǿ��� ���¸� �����ϴ� �۾��� ���� ó�� ����
	//TODO : �����ؼ� ������ ��� ���� �������־����.
	ItemDatas.ItemPlace = EItemPlace::AROUND;
	SetOwnerCharacter(nullptr);               //OwnerCharacter ����
	SetActorHiddenInGame(false);			  //����� ���̵��� Hidden ����.
	SetActorEnableCollision(true);			  //Overlap���� �ϵ��� Collision On
	//Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);//�̰� ��ô���� ����ϴ� ���.

	//�ٴ� ���� ĳ���� �޾ƿͼ� �ٴڿ� ������ �����ϱ�.
	SetActorLocation(GetGroundLocation(Character) + RootComponent->Bounds.BoxExtent.Z);
}
