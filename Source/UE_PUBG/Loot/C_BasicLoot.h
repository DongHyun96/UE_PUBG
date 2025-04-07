// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_BasicLoot.generated.h"

class AC_Item;

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

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<AC_Item*> LootItems{};
};
