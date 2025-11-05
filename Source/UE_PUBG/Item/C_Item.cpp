// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/C_Item.h"
#include "Utility/C_Util.h"
//#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_InvenSystem.h"
#include "InvenUI/C_InventoryUIWidget.h"
#include "Engine/World.h"

#include "Loot/C_BasicLoot.h"

//#include "GameFramework/Actor.h"
//#include "Components/StaticMeshComponent.h"

#include "Weapon/WeaponStrategy/I_WeaponButtonStrategy.h"
#include "Weapon/WeaponStrategy/C_GunStrategy.h"

#include "Singleton/C_GameSceneManager.h"
#include "Singleton/C_GameInstance.h"

// Sets default values
AC_Item::AC_Item()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bCanEverTick = false;
    //WeaponButtonStrategy = CreateDefaultSubobject<AC_GunStrategy>("GunStrategy");

	//OwnerCharacter = nullptr;
}

void AC_Item::BeginPlay()
{
	Super::BeginPlay();

	InitializeItem(ItemCode);

	// Init PrimitiveComponents
	GetComponents<UPrimitiveComponent>(PrimitiveComponents);
}

void AC_Item::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	/*if (EndPlayReason == EEndPlayReason::Type::)
	OnRespawnableItemPickedUp.Unbind();*/
}

void AC_Item::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_Item::InitializeItem(FName NewItemCode)
{
	static const FString ContextString(TEXT("Item Lookup"));
	//TODO : 나중에 ItemManager를 통해 아이템을 모두 관리하게 되면 ItemManager를 통해서 ItemDataRef 정의해 주기.
	//UDataTable* ItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Project_PUBG/Common/Item/ItemDataTables/DT_Item.DT_Item"));

	UC_GameInstance* GI = Cast<UC_GameInstance>(GetGameInstance());

	UDataTable* ItemDataTable = GI->GetDataTables()[EDataTableType::Item];

	if (ItemDataTable)
	{
		const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(ItemCode, ContextString);
		if (ItemData)
		{
			ItemDataRef = ItemData;  // 원본 참조 저장
		}
	}

	// TODO : SoundDataTable 초기화 처리
	// UDataTable* SoundDataTable = GI->GetDataTables()[EDataTableType]
}

void AC_Item::DetachItem()
{
	
	if (!OwnerCharacter) return;
	

	//후에 라인 트레이스를 사용해서 바꿔주기.
	//SetActorLocation(OwnerCharacter->GetActorLocation() + FVector(0.f, 0.f, -75.f));
	DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	this->SetActorLocation(GetGroundLocation(OwnerCharacter) + FVector(0,0,5));// +RootComponent->Bounds.BoxExtent.Z);

	SetOwnerCharacter(nullptr);
	SetActorRotation(FRotator(0.f, 0.f, -90.f));

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
		if (IsValid(HitResult.GetActor())) UC_Util::Print(HitResult.GetActor()->GetName());
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
	AC_Item* SpawnItem = GetWorld()->SpawnActor<AC_Item>(
		this->GetClass(), 
		GetGroundLocation(Character) + RootComponent->Bounds.BoxExtent.Z, 
		Character->GetActorRotation(), 
		SpawnParams
	);

	//SpawnItem->SetItemStack(1);
	//SpawnItem->SetActorHiddenInGame(true);
	SpawnItem->SetActorEnableCollision(false);//생성될 때 무조건 OverlapBegine에 반응해서 우선 꺼뒀음.
	GAMESCENE_MANAGER->AddSpawnedItemToContainer(this);

	return SpawnItem;
}

bool AC_Item::MoveToInven(AC_BasicCharacter* Character, int32 InStack)
{
	bool bIsMoveItem = false;

	switch (ItemPlace)
	{
	case EItemPlace::AROUND:
		bIsMoveItem = MoveAroundToInven(Character, InStack);
		break;
	case EItemPlace::INVEN:
		bIsMoveItem = MoveInvenToInven(Character, InStack);
		break;
	case EItemPlace::SLOT:
		bIsMoveItem = MoveSlotToInven(Character, InStack);
		break;
	default:
		break;
	}
	
	if (bIsMoveItem)
	{
		// 인벤에 넣으면 tick 켜주기.
		SetActorTickEnabled(true);

		// 정상적으로 파밍이 되었을 때, Respawn 처리가 걸린 아이템의 경우 Respawn 담당 Delegate 호출 처리
		OnRespawnableItemPickedUp.ExecuteIfBound(this);
	}

	AC_BasicLoot* OwnerLootBox = Cast<AC_BasicLoot>(this->GetOwner());

	//이 부분에 분명히 아이템 나눠지면서 생기는 문제가 생길 것 같음.
	if (OwnerLootBox && bIsMoveItem)
	{
		OwnerLootBox->RemoveLootItem(this);
		if (Character->GetInvenComponent()->GetAroundItems().Contains(this))
			Character->GetInvenComponent()->RemoveItemToAroundList(this);
	}

	if (AC_Player* Player = Cast<AC_Player>(Character))
	{
		//Player->GetInvenSystem()->GetInvenUI()->UpdateEquipmentItemPanelWidget();
		//Player->GetInvenSystem()->GetInvenUI()->UpdateAroundItemPanelWidget();
		Player->GetInvenSystem()->GetInvenUI()->UpdateWidget();
	}



	return bIsMoveItem;
}

bool AC_Item::MoveToAround(AC_BasicCharacter* Character, int32 InStack)
{
	bool bIsMoveItem = false;

	switch (ItemPlace)
	{
	case EItemPlace::AROUND:
		bIsMoveItem = MoveAroundToAround(Character, InStack);
		break;
	case EItemPlace::INVEN:
		bIsMoveItem = MoveInvenToAround(Character, InStack);
		break;
	case EItemPlace::SLOT:
		bIsMoveItem = MoveSlotToAround(Character, InStack);
		break;
	default:
		break;
	}
	// 아이템을 땅에 버리면 Tick 꺼주기.
	if (bIsMoveItem == true) SetActorTickEnabled(false);

	return bIsMoveItem;
}

bool AC_Item::MoveToSlot(AC_BasicCharacter* Character, int32 InStack)
{
	bool bIsMoveItem = false;
	switch (ItemPlace)
	{
	case EItemPlace::AROUND:
		bIsMoveItem = MoveAroundToSlot(Character, InStack);
		break;
	case EItemPlace::INVEN:
		bIsMoveItem = MoveInvenToSlot(Character, InStack);
		break;
	case EItemPlace::SLOT:
		bIsMoveItem = MoveSlotToSlot(Character, InStack);
		break;
	default:
		break;
	}

	if (bIsMoveItem)
	{
		// 인벤에 넣으면 tick 켜주기.
		SetActorTickEnabled(true);

		// 정상적으로 파밍이 되었을 때, Respawn 처리가 걸린 아이템의 경우 Respawn 담당 Delegate 호출 처리
		OnRespawnableItemPickedUp.ExecuteIfBound(this);
	}

	AC_BasicLoot* OwnerLootBox = Cast<AC_BasicLoot>(this->GetOwner());

	//이 부분에 분명히 아이템 나눠지면서 생기는 문제가 생길 것 같음.
	if (OwnerLootBox && bIsMoveItem == true)
	{
		OwnerLootBox->RemoveLootItem(this);
		if (Character->GetInvenComponent()->GetAroundItems().Contains(this))
			Character->GetInvenComponent()->RemoveItemToAroundList(this);

	}

	if (AC_Player* Player = Cast<AC_Player>(Character))
	{
		Player->GetInvenSystem()->GetInvenUI()->UpdateEquipmentItemPanelWidget();
		Player->GetInvenSystem()->GetInvenUI()->UpdateAroundItemPanelWidget();
	}

	return bIsMoveItem;
}

bool AC_Item::MoveSlotToAround(AC_BasicCharacter* Character, int32 InStack)
{
	return false;
}
bool AC_Item::MoveSlotToInven(AC_BasicCharacter* Character, int32 InStack)
{
	return false;
}

bool AC_Item::MoveSlotToSlot(AC_BasicCharacter* Character, int32 InStack)
{
	return false;
}

bool AC_Item::MoveInvenToAround(AC_BasicCharacter* Character, int32 InStack)
{
	return false;
}

bool AC_Item::MoveInvenToInven(AC_BasicCharacter* Character, int32 InStack)
{
	return false;
}			  
bool AC_Item::MoveInvenToSlot(AC_BasicCharacter* Character, int32 InStack)
{
	return false;
}

bool AC_Item::MoveAroundToAround(AC_BasicCharacter* Character, int32 InStack)
{
	return false;
}

bool AC_Item::MoveAroundToInven(AC_BasicCharacter* Character, int32 InStack)
{
	return false;
}

bool AC_Item::MoveAroundToSlot(AC_BasicCharacter* Character, int32 InStack)
{
	return false;
}

void AC_Item::DropItem(AC_BasicCharacter* Character)
{
	//TODO : 아이템이 장착(Attach)되었던 상태를 해제하는 작업에 관한 처리 생각
	//TODO : 분할해서 버리는 경우 새로 스폰해주어야함.
	ItemPlace = EItemPlace::AROUND;

	SetOwnerCharacter(nullptr);               //OwnerCharacter 해제
	SetActorHiddenInGame(false);			  //모습이 보이도록 Hidden 해제.

	SetActorEnableCollision(false);			  //Attachable Item 이 Around로 갈 때 바로 List에 추가가 안되서 껏다 켜줌.
	SetActorEnableCollision(true);			  //Overlap가능 하도록 Collision On
	//Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);//이건 투척류만 사용하는 기능.

	//바닥 레이 캐스팅 받아와서 바닥에 아이템 생성하기.
	this->SetActorLocation(GetGroundLocation(Character) + RootComponent->Bounds.BoxExtent.Z);
}

void AC_Item::SetItemStack(int32 inItemStack)
{
	ItemCurStack = inItemStack;
	
	if (!OwnerCharacter) return;

	if (ItemCurStack == 0)
	{
		OwnerCharacter->GetInvenComponent()->DestroyMyItem(this);
	}
}

void AC_Item::SetOutlineEffect(bool bEnable)
{
	for (UPrimitiveComponent* Comp : PrimitiveComponents)
	{
		if (Comp)
		{
			Comp->SetRenderCustomDepth(bEnable);
			Comp->SetCustomDepthStencilValue(bEnable ? 1 : 0);
		}
	}
}

AC_Item* AC_Item::DividItemSpawn(int32 DivideNum, AC_BasicCharacter* Character, bool ActorEnAbleCollision)
{
	if (ItemCurStack == 1) return nullptr; // 갯수가 1이면 나누기 불가. nullptr 반환.

	AC_Item* DividedItem = this->SpawnItem(Character);

	this->SetItemStack(ItemCurStack - DivideNum);

	DividedItem->SetItemStack(DivideNum);

	DividedItem->SetActorEnableCollision(ActorEnAbleCollision);

	return DividedItem;
}

bool AC_Item::DestroyItem(bool bNetForce, bool bShouldModifyLevel)
{
	// 아이템 매니저에서 아이템 제거.
	GAMESCENE_MANAGER->DeleteSpawnedItemFromContainer(this);

	// 아이템 객체 제거.
	return this->Destroy(bNetForce, bShouldModifyLevel);
}
