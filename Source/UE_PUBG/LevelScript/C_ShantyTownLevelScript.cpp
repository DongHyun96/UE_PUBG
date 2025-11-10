// Fill out your copyright notice in the Description page of Project Settings.


#include "C_ShantyTownLevelScript.h"

#include "EngineUtils.h"
#include "Algo/RandomShuffle.h"
#include "Engine/StaticMeshActor.h"
#include "Item/C_Item.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Weapon/MeleeWeapon/C_MeleeWeapon.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Item/ItemManager/C_ItemManager.h"
#include "Singleton/C_GameInstance.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"


AC_ShantyTownLevelScript::AC_ShantyTownLevelScript()
{
	//PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bCanEverTick = false;

}


void AC_ShantyTownLevelScript::BeginPlay()
{
	Super::BeginPlay();

	if (!bUseCodeToInit) return;

	if (UC_GameInstance* GameInstance = Cast<UC_GameInstance>(GetGameInstance()))
		GameInstance->SetCurrentSelectedLevelType(ELevelType::ShantyTown);
	else UC_Util::Print("From AC_ShantyTownLevelScript::BeginPlay : GameInstance casting failed!", FColor::Red, 10.f);

	UC_Util::Print("Using LevelScript to spawn level items", FColor::Red, 10.f);
	//return;
	InitFloors();

	GAMESCENE_MANAGER->GetItemManager()->PoolingWeightedItemCodes();

	//SpawnItemInWorld(GAMESCENE_MANAGER->GetItemManager()->GetWeightedItemCodes());
	LegacySpawnItem(GAMESCENE_MANAGER->GetItemManager()->GetWeightedItemCodes());
	//FString str = "ShantyTown Floor Count : " + FString::FromInt(FloorStaticMeshComponents.Num()); 
	//UC_Util::Print(str , FColor::Yellow, 10.f);
	//UC_Util::Print("Spawned Items Count : " + FString::FromInt(ItemLocationsInAComponent.Num()), FColor::Emerald, 10.f);
	UC_Util::Print("Spawned Items Count : " + FString::FromInt(GAMESCENE_MANAGER->GetItemManager()->GetItemContainer().Num()),FColor::Emerald, 10);

	GAMESCENE_MANAGER->ToggleVehiclesHiddenInGame(bHideSpawnedItemsOnGameStart);
	GAMESCENE_MANAGER->GetItemManager()->ToggleItemsHiddenInGame(bHideSpawnedItemsOnGameStart);
}

void AC_ShantyTownLevelScript::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_ShantyTownLevelScript::InitFloors()
{
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (!IsValid(Actor)) continue;

		// 단일 StaticMeshActor, Floor인지 검사
		if (AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(Actor))
		{
			// 해당 StaticMeshActor가 바닥면인지 조사해서 Array에 추가
			UStaticMeshComponent* StaticMeshComponent = StaticMeshActor->GetStaticMeshComponent();

			if (IsValid(StaticMeshComponent)) TryAddToFloorStaticMeshComponentsArrayIfPossible(StaticMeshComponent);
		}

		for (TSubclassOf<AActor> BuildingClass : ShantyTownBuildingClasses)
		{
			if (!Actor->IsA(BuildingClass)) continue;

			// Building Class인 Actor인 상황
			
			// 바닥면인지 확인해서 바닥면이면 Array에 추가
			TArray<UStaticMeshComponent*> StaticMeshComponents{};
			Actor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

			for (UStaticMeshComponent* StaticMeshComponent : StaticMeshComponents)
				TryAddToFloorStaticMeshComponentsArrayIfPossible(StaticMeshComponent);
		}
	}

	Algo::RandomShuffle(FloorStaticMeshComponents);
}

void AC_ShantyTownLevelScript::TryAddToFloorStaticMeshComponentsArrayIfPossible(UStaticMeshComponent* StaticMeshComponent)
{
	if (StaticMeshComponent->GetStaticMesh()->GetName().Contains("Floor"))
		FloorStaticMeshComponents.Add(StaticMeshComponent);
	// if (StaticMeshComponent->GetName().Contains("Floor"))
}

void AC_ShantyTownLevelScript::LegacySpawnItem(const TArray<FName> ItemNameList)
{
	const int32 MaxItemsPerFloor = 5;

	const TArray<FName> WeightedItemCodes = ItemNameList;


	for (UStaticMeshComponent* FloorMeshComponent : FloorStaticMeshComponents)
	{
		FVector Origin{}, BoxExtent{};
		float SphereRadius{};
		UKismetSystemLibrary::GetComponentBounds(FloorMeshComponent, Origin, BoxExtent, SphereRadius);
		float ZOffset = BoxExtent.Z * 0.5f;

		SpawnCount = 0;

		while (!bCanSpawn && SpawnCount < MaxItemsPerFloor && WeightedItemCodes.Num() > 0)
		{
			float RandomX = FMath::RandRange(-BoxExtent.X, BoxExtent.X) * 0.5f;
			float RandomY = FMath::RandRange(-BoxExtent.Y, BoxExtent.Y) * 0.5f;

			FVector FloorWorldLocation = FloorMeshComponent->GetComponentLocation();

			NewSpawnLocation = FloorWorldLocation + FVector::UnitX() * RandomX + FVector::UnitY() * RandomY;

			for (const FVector& ItemLocation : ItemLocationsInAComponent)
			{
				if (FVector::Distance(ItemLocation, NewSpawnLocation) < 5.f)
				{
					++SpawnCount;
					break;
				}
			}
			bCanSpawn = true;

			if (!bCanSpawn) continue;

			ItemLocationsInAComponent.Add(NewSpawnLocation);

			FActorSpawnParameters SpawnParams{};
			FName ItemCode = WeightedItemCodes[FMath::RandRange(0, WeightedItemCodes.Num() - 1)];

			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			//AC_Item* SpawnedItem = GetWorld()->SpawnActor<AC_Item>(ItemClass, NewSpawnLocation, FRotator::ZeroRotator, SpawnParams);
			AC_Item* SpawnedItem = GAMESCENE_MANAGER->GetItemManager()->SpawnItem(ItemCode, NewSpawnLocation);
			++SpawnCount;
			bCanSpawn = false;
			//SpawnCount = 0;
			ItemLocationsInAComponent.Empty();

			const FVector SpawnedLocation = SpawnedItem->GetActorLocation();

			UMeshComponent* ItemMeshComponent = SpawnedItem->GetComponentByClass<UMeshComponent>();
			FVector ItemMeshOrigin{}, ItemMeshBoxExtent{};
			float ItemMeshSphereRadius{};
			UKismetSystemLibrary::GetComponentBounds(ItemMeshComponent, ItemMeshOrigin, ItemMeshBoxExtent, ItemMeshSphereRadius);
			const float ItemMeshBoxExtentY = ItemMeshBoxExtent.Y;

			if (AC_Gun* Gun = Cast<AC_Gun>(SpawnedItem))
			{
				Gun->SetActorLocation({ SpawnedLocation.X, SpawnedLocation.Y, ItemMeshBoxExtentY * 1.5f + SpawnedLocation.Z });
				// Gun->SetActorRotation({90.f, 0.f, 0.f});
				Gun->SetActorRotation({ 0.f, 90.f, 90.f });

				// Spawn Bullets
				TSubclassOf<AC_Item_Bullet> BulletClass = PUBGBulletClasses[Gun->GetCurBulletType()];

				FTransform BulletTransform{};
				BulletTransform.SetLocation(Gun->GetActorLocation());
				BulletTransform.SetScale3D({ 0.5f, 0.5f, 0.5f });

				//AC_Item_Bullet* SpawnedBullet = GetWorld()->SpawnActor<AC_Item_Bullet>(BulletClass, BulletTransform, SpawnParams);
				AC_Item* SpawnedBullet = GAMESCENE_MANAGER->GetItemManager()->SpawnItem(Gun->GetCurrentBulletTypeName(), Gun->GetActorLocation());

				//GAMESCENE_MANAGER->AddSpawnedItemToContainer(SpawnedItem);
				//GAMESCENE_MANAGER->AddSpawnedItemToContainer(SpawnedBullet);
				Gun->SetActorHiddenInGame(bHideSpawnedItemsOnGameStart);
				SpawnedBullet->SetActorHiddenInGame(bHideSpawnedItemsOnGameStart);
				continue;
			}

			if (AC_MeleeWeapon* MeleeWeapon = Cast<AC_MeleeWeapon>(SpawnedItem))
			{
				MeleeWeapon->SetActorLocation({ SpawnedLocation.X, SpawnedLocation.Y, ItemMeshBoxExtentY * 1.5f + SpawnedLocation.Z });
				//MeleeWeapon->SetActorRotation({ 90.f, -55.f, 0.f });
				MeleeWeapon->SetActorRotation({ 125.f, 0, 90.f });
				// 
				//GAMESCENE_MANAGER->AddSpawnedItemToContainer(MeleeWeapon);
				MeleeWeapon->SetActorHiddenInGame(bHideSpawnedItemsOnGameStart);
				continue;
			}

			SpawnedItem->SetActorLocation({ SpawnedLocation.X, SpawnedLocation.Y, ZOffset + SpawnedLocation.Z });
			//GAMESCENE_MANAGER->AddSpawnedItemToContainer(SpawnedItem);
			SpawnedItem->SetActorHiddenInGame(bHideSpawnedItemsOnGameStart);
		}
	}
}

/*
void AC_ShantyTownLevelScript::SpawnItemInWorld(const TArray<FName> ItemNameList)
{
	const int32 MaxItemsPerFloor = 4;

	const TArray<FName> WeightedItemCodes = ItemNameList;

	for (UStaticMeshComponent* FloorMesh : FloorStaticMeshComponents)
	{
		FVector Origin, Extent;
		float Radius;
		UKismetSystemLibrary::GetComponentBounds(FloorMesh, Origin, Extent, Radius);

		FVector FloorLocation = FloorMesh->GetComponentLocation();
		float ZOffset = Extent.Z * 0.5f;

		int32 SpawnedCount = 0;
		TArray<FVector> UsedLocations;

		while (SpawnedCount < MaxItemsPerFloor && WeightedItemCodes.Num() > 0)
		{
			// 랜덤 아이템 선택
			FName ItemCode = WeightedItemCodes[FMath::RandRange(0, WeightedItemCodes.Num() - 1)];
			//const FItemData& ItemData = GeneralItemDataCache[ItemCode];

			FVector2D RandXY = FMath::RandPointInCircle(Extent.X * 0.5f);
			FVector SpawnLoc = FloorLocation + FVector(RandXY.X, RandXY.Y, ZOffset);

			bool bTooClose = false;
			for (const FVector& Prev : UsedLocations)
			{
				if (FVector::Distance(Prev, SpawnLoc) < 10.f)
				{
					bTooClose = true;
					break;
				}
			}
			if (bTooClose) continue;

			AC_Item* SpawnedItem = GAMESCENE_MANAGER->GetItemManager()->SpawnItem(ItemCode, SpawnLoc);

			// 스폰
			//AC_Item* Item = SpawnItem(ItemCode, SpawnLoc, 1);
			if (!SpawnedItem) continue;

			SpawnedItem->SetActorHiddenInGame(bHideSpawnedItemsOnGameStart);

			UsedLocations.Add(SpawnLoc);
			++SpawnedCount;

			const FVector SpawnedLocation = SpawnedItem->GetActorLocation();

			UMeshComponent* ItemMeshComponent = SpawnedItem->GetComponentByClass<UMeshComponent>();
			FVector ItemMeshOrigin{}, ItemMeshBoxExtent{};
			float ItemMeshSphereRadius{};
			UKismetSystemLibrary::GetComponentBounds(ItemMeshComponent, ItemMeshOrigin, ItemMeshBoxExtent, ItemMeshSphereRadius);
			const float ItemMeshBoxExtentY = ItemMeshBoxExtent.Y;

			if (AC_Gun* Gun = Cast<AC_Gun>(SpawnedItem))
			{
				Gun->SetActorLocation({ SpawnedLocation.X, SpawnedLocation.Y, ItemMeshBoxExtentY * 1.5f + SpawnedLocation.Z });
				Gun->SetActorRotation({ 0.f, 90.f, 90.f });

				GAMESCENE_MANAGER->GetItemManager()->SpawnItem(Gun->GetCurrentBulletTypeName(), Gun->GetActorLocation());

				continue;
			}

			if (AC_MeleeWeapon* MeleeWeapon = Cast<AC_MeleeWeapon>(SpawnedItem))
			{
				MeleeWeapon->SetActorLocation({ SpawnedLocation.X, SpawnedLocation.Y, ItemMeshBoxExtentY * 1.5f + SpawnedLocation.Z });
				MeleeWeapon->SetActorRotation({ 125.f, 0, 90.f });
				continue;
			}

			SpawnedItem->SetActorLocation({ SpawnedLocation.X, SpawnedLocation.Y, ZOffset + SpawnedLocation.Z });

		}
	}
}
*/

