// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ConsumableItem/C_ConsumableItem.h"
#include "C_BoostItem.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_BoostItem : public AC_ConsumableItem
{
	GENERATED_BODY()
	
public:

	AC_BoostItem();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
private: // Primitive Operations

	bool IsAvailableToStartUsing(AC_BasicCharacter* InItemUser) override;

	void HandleActivatingState() override;

	void HandleActivateCompletedState() override;

	void HandleDestroy() override;

protected:

	float BoostAmount{};

};


