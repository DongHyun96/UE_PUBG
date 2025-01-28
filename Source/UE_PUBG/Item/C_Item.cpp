// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/C_Item.h"
#include "Utility/C_Util.h"
#include "Character/C_BasicCharacter.h"
#include "Character/Component/C_InvenComponent.h"
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

void AC_Item::DetachItem()
{
	
	if (!OwnerCharacter) return;
	
	DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);

	//후에 라인 트레이스를 사용해서 바꿔주기.
	SetActorLocation(OwnerCharacter->GetActorLocation() + FVector(0.f, 0.f, -75.f));
	SetActorRotation(FRotator(0.f, 0.f, -90.f));
	SetOwnerCharacter(nullptr);

	//한번 껏다 꺼줘야 OverlapBegin이 작동 -> 장착할때 꺼주고 버릴 때 켜주면 될듯?
	SetActorEnableCollision(false);
	SetActorEnableCollision(true);
	
	

	UC_Util::Print("Check");
}

void AttachToSocket(USceneComponent* InParent)
{

}

FVector AC_Item::GetGroundLocation(AC_BasicCharacter* Character)
{
	// 캐릭터의 위치
	FVector CharacterLocation = Character->GetActorLocation();

	// 라인 트레이스 시작 및 끝 위치 설정
	FVector TraceStart = CharacterLocation;
	FVector TraceEnd = TraceStart - FVector(0.0f, 0.0f, 10000.0f); // 아래 방향으로 10,000 유닛

	// 라인 트레이스 설정
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character); // 캐릭터는 무시

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_Visibility, // 충돌 채널
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
	//location, rotation을 this의 것을 쓰는 것도 생각, 왜냐하면 지금 이상하게 날라가는 이유가 이것일 수 도 있음. -> 섬광탄이 터지고 충돌체가 남아있음.
	AC_Item* SpawnItem = GetWorld()->SpawnActor<AC_Item>(this->GetClass(), GetGroundLocation(Character) + RootComponent->Bounds.BoxExtent.Z, Character->GetActorRotation(), SpawnParams);
	//SpawnItem->SetItemStack(1);
	//SpawnItem->SetActorHiddenInGame(true);
	SpawnItem->SetActorEnableCollision(false);//생성될 때 무조건 OverlapBegine에 반응해서 우선 꺼뒀음.
	return SpawnItem;
}

bool AC_Item::MoveToInven(AC_BasicCharacter* Character)
{
	
	switch (GetItemDatas().ItemPlace)
	{
	case EItemPlace::AROUND:
		return MoveAroundToInven(Character);
	case EItemPlace::INVEN:
		return MoveInvenToInven(Character);
	case EItemPlace::SLOT:
		return MoveSlotToInven(Character);
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
		return MoveAroundToAround(Character);
	case EItemPlace::INVEN:
		return MoveInvenToAround(Character);
	case EItemPlace::SLOT:
		return MoveSlotToAround(Character);
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
		return MoveAroundToSlot(Character);
	case EItemPlace::INVEN:
		return MoveInvenToSlot(Character);
	case EItemPlace::SLOT:
		return MoveSlotToSlot(Character);
	default:
		break;
	}
	return false;
}

bool AC_Item::MoveSlotToAround(AC_BasicCharacter* Character)
{
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
	//TODO : 아이템이 장착(Attach)되었던 상태를 해제하는 작업에 관한 처리 생각
	//TODO : 분할해서 버리는 경우 새로 스폰해주어야함.
	ItemDatas.ItemPlace = EItemPlace::AROUND;

	SetOwnerCharacter(nullptr);               //OwnerCharacter 해제
	SetActorHiddenInGame(false);			  //모습이 보이도록 Hidden 해제.
	SetActorEnableCollision(false);			  
	SetActorEnableCollision(true);			  //Overlap가능 하도록 Collision On
	//Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);//이건 투척류만 사용하는 기능.

	//바닥 레이 캐스팅 받아와서 바닥에 아이템 생성하기.
	this->SetActorLocation(GetGroundLocation(Character) + RootComponent->Bounds.BoxExtent.Z);
}

void AC_Item::SetItemStack(uint8 inItemStack)
{
	ItemDatas.ItemCurStack = inItemStack;
	
	if (!OwnerCharacter) return;

	if (ItemDatas.ItemCurStack == 0)
	{
		OwnerCharacter->GetInvenComponent()->DestroyMyItem(this);
	}
}
