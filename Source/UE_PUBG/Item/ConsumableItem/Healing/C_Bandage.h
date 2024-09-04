// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ConsumableItem/C_ConsumableItem.h"
#include "C_Bandage.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_Bandage : public AC_ConsumableItem
{
	GENERATED_BODY()

public:
	AC_Bandage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	bool StartUsingConsumableItem(class AC_BasicCharacter* InItemUser) override;
	
	void HandleActivatingState()		override;
	void HandleActivateCompletedState() override;

private:

	const float ONE_BLOCK_HEAL_AMOUNT = 2.5f;
	const float ONE_BLOCK_TIME		  = 0.375f;
	const UINT	TOTAL_USING_BLOCK_CNT = 4;

private:

	// รั ภฬฟ๋ Block
	UINT BlockUsed{};
};
