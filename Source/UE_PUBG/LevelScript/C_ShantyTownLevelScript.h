// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "C_ShantyTownLevelScript.generated.h"

enum class EBulletType : uint8;
/// <summary>
/// 기존 Shanty-town 레벨 블루프린트 가독성이 너무 안좋아서 c++ 코드로 옮김
/// </summary>
UCLASS()
class UE_PUBG_API AC_ShantyTownLevelScript : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	
	AC_ShantyTownLevelScript();

protected:
	
	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;

private:

	void InitFloors();

	void TryAddToFloorStaticMeshComponentsArrayIfPossible(UStaticMeshComponent* StaticMeshComponent);

	void LegacySpawnItem(const TArray<FName> ItemNameList);


	void SpawnItemInWorld(const TArray<FName> ItemNameList);


protected:
	// Building 클래스 references
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<TSubclassOf<AActor>> ShantyTownBuildingClasses{};
	// Spawn 처리될 아이템 클래스 종류들 (블루프린트에서 모두 넣어주어야 함)
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<TSubclassOf<class AC_Item>> PUBGItemClasses{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EBulletType, TSubclassOf<class AC_Item_Bullet>> PUBGBulletClasses{};
	
private:
	
	TArray<AStaticMeshActor*> StaticMeshes{};
	// TArray<UStaticMeshComponent*> StaticMeshComponents{}; // 이게 바닥면 StaticMeshComponent 였었음

	TArray<UStaticMeshComponent*> FloorStaticMeshComponents{};

private://Legacy

	int SpawnCount{};
	bool bCanSpawn{};
	FVector NewSpawnLocation{};
	TArray<FVector> ItemLocationsInAComponent{};


protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool bUseCodeToInit{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool bHideSpawnedItemsOnGameStart{};
	
};

