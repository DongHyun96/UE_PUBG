// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ConsumableItem/C_ConsumableItem.h"
#include "C_HealItem.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_HealItem : public AC_ConsumableItem
{
	GENERATED_BODY()
	
public:

	AC_HealItem();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:

	void HandleActivateCompletedState() override; // Hook Method

	bool IsAvailableToStartUsing(class AC_BasicCharacter* InItemUser) override;

protected:

	float OneBlockHealAmount{};
	float OneBlockTime{};
	UINT  TotalUsingBlockCnt{};

	float HealUpLimit{};

	// �� �̿� Block
	UINT BlockUsed{};

};