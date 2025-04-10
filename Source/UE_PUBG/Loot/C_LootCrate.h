// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Loot/C_BasicLoot.h"
#include "C_LootCrate.generated.h"

UCLASS()
class UE_PUBG_API AC_LootCrate : public AC_BasicLoot
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AC_LootCrate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/// <summary>
	/// 죽은 캐릭터의 모든 아이템을 LootCarate에 넣어주는 함수.
	/// </summary>
	/// <param name="InCharacter">죽은 캐릭터</param>
	/// <returns>true면 무사히 아이템을 넣음, false면 캐릭터가 nullptr</returns>
	bool SetCharacterLootCrate(class AC_BasicCharacter* InCharacter);

protected:
	/// <summary>
	/// 캐릭터의 Inventory의 MyItems를 LootItems에 Add시키는 함수.
	/// </summary>
	/// <param name="InvenComp"></param>
	void AddInventoryItemsToLoot(class UC_InvenComponent* InvenComp);

	/// <summary>
	/// 캐릭터의 InvenComponent의 EquipmentItem의 아이템을 LootItems에 Add시키는 함수.
	/// </summary>
	/// <param name="InvenComp"></param>
	void AddEquipmentItemsToLoot(UC_InvenComponent* InvenComp);

	/// <summary>
	/// 캐릭터의 EquippedComponent의 Weapons를 LootItems에 Add시키는 함수.
	/// </summary>
	/// <param name="EquipComp"></param>
	void AddEquippedWeaponsToLoot(class UC_EquippedComponent* EquipComp);
};
