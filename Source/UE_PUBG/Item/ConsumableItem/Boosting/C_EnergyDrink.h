// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ConsumableItem/C_ConsumableItem.h"
#include "C_EnergyDrink.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_EnergyDrink : public AC_ConsumableItem
{
	GENERATED_BODY()
	
public:
	AC_EnergyDrink();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	bool StartUsingConsumableItem(class AC_BasicCharacter* InItemUser) override;

	void HandleActivatingState() override;
	void HandleActivateCompletedState() override;
	
private:

	const float BOOST_AMOUNT = 40.f;

};
