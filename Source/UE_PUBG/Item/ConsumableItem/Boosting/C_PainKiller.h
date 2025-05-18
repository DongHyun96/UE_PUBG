// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ConsumableItem/C_BoostItem.h"
#include "C_PainKiller.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_PainKiller : public AC_BoostItem
{
	GENERATED_BODY()

public:
	AC_PainKiller();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//bool Interaction(AC_BasicCharacter* Character) override;
 
private:// Primitive Operations

	void OnStartUsing()		  override;
	void OnActivatingFinish() override;
	void OnCancelActivating() override;
};


