// Fill out your copyright notice in the Description page of Project Settings.


#include "Loot/C_BasicLoot.h"
#include "Item/C_Item.h"
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

void AC_BasicLoot::CheckLootItems()
{
	for (AC_Item* Item : LootItems)
	{
		if (Item->GetOwnerCharacter())
			LootItems.Remove(Item);
	}
}
