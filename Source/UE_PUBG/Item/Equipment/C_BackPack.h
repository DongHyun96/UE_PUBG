// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "C_BackPack.generated.h"

/**
 * 배낭 (Level 1): 소지 용량 +150
 * 배낭 (Level 2): 소지 용량 +200
 * 배낭 (Level 3)(보급 전용 3레벨 배낭도 포함): 소지 용량 +250
 */


UCLASS()
class UE_PUBG_API AC_BackPack : public AC_EquipableItem
{
	GENERATED_BODY()

public:

	AC_BackPack();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Interaction(class AC_BasicCharacter* character) override;

	uint8 GetLevel() { return Level; }
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 Level;
};
