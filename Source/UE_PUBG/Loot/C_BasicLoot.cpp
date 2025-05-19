// Fill out your copyright notice in the Description page of Project Settings.


#include "Loot/C_BasicLoot.h"
#include "Item/C_Item.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"
// Sets default values
AC_BasicLoot::AC_BasicLoot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AC_BasicLoot::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
	InitializeLootItems();
}

// Called every frame
void AC_BasicLoot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckLootItems();
}

void AC_BasicLoot::InitializeLootItems()
{
	for (AC_Item* Item : LootItems)
	{
		Item->SetOwner(this);
	}
}

void AC_BasicLoot::AddItemInLootItems(AC_Item* InItem)
{
	FName InItemCode = InItem->GetItemCode();
	AC_Item* CachedItem = nullptr;
	for (AC_Item* Item : LootItems)
	{
		FName CurItemCode = Item->GetItemCode();

		if (InItemCode == CurItemCode)
		{
			if (!CachedItem)
				CachedItem = Item;
			if (CachedItem->GetItemCurStack() > Item->GetItemCurStack())
				CachedItem = Item;
			//int32 ItemMaxStack = Item->GetItemDatas()->ItemMaxStack;
			//if (Item->GetItemCurStack() == ItemMaxStack) continue;

		}
	}

	if (CachedItem) 
	{
		int32 PrevItemStack = CachedItem->GetItemCurStack() + InItem->GetItemCurStack();
		int32 CachedItemMaxStack = CachedItem->GetItemDatas()->ItemMaxStack;

		if (PrevItemStack > CachedItemMaxStack)
		{
			PrevItemStack -= CachedItemMaxStack;
			CachedItem->SetItemStack(CachedItemMaxStack);
			InItem->SetItemStack(PrevItemStack);
		}

		CachedItem->SetItemStack(PrevItemStack);
		InItem->DestroyItem();
	}
	else
		LootItems.Emplace(InItem);
}

void AC_BasicLoot::CheckLootItems()
{
	for (AC_Item* Item : LootItems)
	{
		if (Item->GetOwnerCharacter())
			LootItems.Remove(Item);
	}
}
