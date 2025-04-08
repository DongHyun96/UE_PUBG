// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_BasicLoot.generated.h"

class AC_Item;

/// <summary>
/// 사용법 : 
/// 1. 미리 Map에 스폰된 아이템을 에디터에서 LootItems에 추가해준다.
/// 2. 해당 블루프린트 클래스에 들어가서 사용할 아이템을 만들고 SetLootItems를 이용한다.
/// </summary>
UCLASS()
class UE_PUBG_API AC_BasicLoot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AC_BasicLoot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	TArray<AC_Item*> GetLootItems() { return LootItems; }

	/// <summary>
	/// 에디터에서 블루프린트 들어가서 ItemList를 만들고 넣어 주면 굿.
	/// </summary>
	/// <param name="ItemList"></param>
	void SetLootItems(TArray<AC_Item*> ItemList) { LootItems.Append(ItemList); }

	/// <summary>
	/// 하나의 아이템을 추가
	/// </summary>
	/// <param name="InItem"></param>
	void SetLootItems(AC_Item* InItem) { LootItems.Emplace(InItem); }

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<AC_Item*> LootItems{};
};
