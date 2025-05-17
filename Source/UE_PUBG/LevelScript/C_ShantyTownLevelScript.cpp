// Fill out your copyright notice in the Description page of Project Settings.


#include "C_ShantyTownLevelScript.h"

#include "EngineUtils.h"
#include "Algo/RandomShuffle.h"
#include "Engine/StaticMeshActor.h"
#include "Item/C_Item.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Weapon/MeleeWeapon/C_MeleeWeapon.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"


AC_ShantyTownLevelScript::AC_ShantyTownLevelScript()
{
	PrimaryActorTick.bCanEverTick = true;
}


void AC_ShantyTownLevelScript::BeginPlay()
{
	Super::BeginPlay();

	if (!bUseCodeToInit) return;
	UC_Util::Print("Using LevelScript to spawn level items", FColor::Red, 10.f);

	InitFloors();

	//FString str = "ShantyTown Floor Count : " + FString::FromInt(FloorStaticMeshComponents.Num()); 
	//UC_Util::Print(str , FColor::Yellow, 10.f);

	// For Testing(지우기)
	float MinBoxExtentSize = 1e9;
	FString MinBoxName{};
	float MaxBoxExtentSize = 0.f;
	float AverageBoxSize{};
	
	for (UStaticMeshComponent* FloorMeshComponent : FloorStaticMeshComponents)
	{
		FVector Origin{}, BoxExtent{};
		float SphereRadius{};
		UKismetSystemLibrary::GetComponentBounds(FloorMeshComponent, Origin, BoxExtent, SphereRadius);
		float ZOffset = BoxExtent.Z * 0.5f;

		float CurrentBoxSize = BoxExtent.X * BoxExtent.Y * 0.01f; // Meter 단위로 처리 
		// MinBoxExtentSize = FMath::Min(MinBoxExtentSize, CurrentBoxSize);
		if (MinBoxExtentSize > CurrentBoxSize)
		{
			MinBoxExtentSize = CurrentBoxSize;
			MinBoxName = FloorMeshComponent->GetOwner()->GetActorLabel();
		}
		
		MaxBoxExtentSize = FMath::Max(MaxBoxExtentSize, CurrentBoxSize);
		AverageBoxSize += CurrentBoxSize;

		const int TOTAL_SPAWN_COUNT_PER_ITEM = 3; // 이게 아이템 총 량은 아니고 같은 아이템 종류 개수 Limit 개수임
		
		for (TSubclassOf<AC_Item> ItemClass : PUBGItemClasses)
		{
			while (!bCanSpawn && SpawnCount < TOTAL_SPAWN_COUNT_PER_ITEM)
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
			}

			if (!bCanSpawn) continue;

			ItemLocationsInAComponent.Add(NewSpawnLocation);
			
			FActorSpawnParameters SpawnParams{};
			
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			AC_Item* SpawnedItem = GetWorld()->SpawnActor<AC_Item>(ItemClass, NewSpawnLocation, FRotator::ZeroRotator, SpawnParams);

			bCanSpawn = false;
			SpawnCount = 0;
			ItemLocationsInAComponent.Empty();

			const FVector SpawnedLocation = SpawnedItem->GetActorLocation();

			UMeshComponent* ItemMeshComponent = SpawnedItem->GetComponentByClass<UMeshComponent>();
			FVector ItemMeshOrigin{}, ItemMeshBoxExtent{};
			float ItemMeshSphereRadius{};
			UKismetSystemLibrary::GetComponentBounds(ItemMeshComponent, ItemMeshOrigin, ItemMeshBoxExtent, ItemMeshSphereRadius);
			const float ItemMeshBoxExtentY = ItemMeshBoxExtent.Y;

			if (AC_Gun* Gun = Cast<AC_Gun>(SpawnedItem))
			{
				Gun->SetActorLocation({SpawnedLocation.X, SpawnedLocation.Y, ItemMeshBoxExtentY * 1.5f + SpawnedLocation.Z});
				// Gun->SetActorRotation({90.f, 0.f, 0.f});

				// Spawn Bullets
				TSubclassOf<AC_Item_Bullet> BulletClass = PUBGBulletClasses[Gun->GetCurBulletType()];

				FTransform BulletTransform{};
				BulletTransform.SetLocation(Gun->GetActorLocation());
				BulletTransform.SetScale3D({0.5f, 0.5f, 0.5f});
				
				AC_Item_Bullet* SpawnedBullet = GetWorld()->SpawnActor<AC_Item_Bullet>(BulletClass, BulletTransform, SpawnParams);

				GAMESCENE_MANAGER->AddSpawnedItemToContainer(SpawnedItem);
				GAMESCENE_MANAGER->AddSpawnedItemToContainer(SpawnedBullet);
				Gun->SetActorHiddenInGame(bHideSpawnedItemsOnGameStart);
				SpawnedBullet->SetActorHiddenInGame(bHideSpawnedItemsOnGameStart);
				continue;
			}

			if (AC_MeleeWeapon* MeleeWeapon = Cast<AC_MeleeWeapon>(SpawnedItem))
			{
				MeleeWeapon->SetActorLocation({SpawnedLocation.X, SpawnedLocation.Y, ItemMeshBoxExtentY * 1.5f + SpawnedLocation.Z});
				MeleeWeapon->SetActorRotation({90.f, -55.f, 0.f});
				GAMESCENE_MANAGER->AddSpawnedItemToContainer(MeleeWeapon);
				MeleeWeapon->SetActorHiddenInGame(bHideSpawnedItemsOnGameStart);
				continue;
			}

			SpawnedItem->SetActorLocation({SpawnedLocation.X, SpawnedLocation.Y, ZOffset + SpawnedLocation.Z});
			GAMESCENE_MANAGER->AddSpawnedItemToContainer(SpawnedItem);
			SpawnedItem->SetActorHiddenInGame(bHideSpawnedItemsOnGameStart);
		}
	}

	AverageBoxSize /= FloorStaticMeshComponents.Num();
	FString str = "AverageFloorSize : " + FString::SanitizeFloat(AverageBoxSize);
	UC_Util::Print(str, FColor::Cyan, 20.f);
	str = "MinFloorSize : " + FString::SanitizeFloat(MinBoxExtentSize);
	UC_Util::Print(str, FColor::Cyan, 20.f);
	str = "MaxFloorSize : " + FString::SanitizeFloat(MaxBoxExtentSize);
	UC_Util::Print(str, FColor::Cyan, 20.f);
	UC_Util::Print(MinBoxName, FColor::Cyan, 20.f);
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

