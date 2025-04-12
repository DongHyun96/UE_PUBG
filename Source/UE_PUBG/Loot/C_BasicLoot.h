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
	/// 하나의 아이템을 추가.
	/// LootItems에 동일한 아이템들을 찾아서 ItemCurStack이 가장 적은 객체에
	/// 접근해서 갯수를 합침.
	/// MaxStack을 넘으면 가장 적은 객체의 ItemCurStack을 MaxStack으로 만들고
	/// 나머지를 InItem의 ItemCurStack으로 Set해주고 LootItems에 Emplace해준다.
	/// 만약 두 아이템을 모두 합쳐도 MaxStack을 넘지 않는다면
	/// LootItems에 있는 아이템에 Stack을 모두 합치고
	/// InItem을 destroy해준다.
	/// </summary>
	/// <param name="InItem"></param>
	void AddItemInLootItems(AC_Item* InItem); //{ LootItems.Emplace(InItem); }

	/// <summary>
	/// 하나의 아이템을 삭제.
	/// </summary>
	/// <param name="InItem"></param>
	void RemoveLootItem(AC_Item* InItem) { LootItems.Remove(InItem); }

protected:
	void CheckLootItems();

	void InitializeLootItems();
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<AC_Item*> LootItems{};
};
