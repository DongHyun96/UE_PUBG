// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ConsumableItem/C_HealItem.h"
#include "C_MedKit.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_MedKit : public AC_HealItem
{
	GENERATED_BODY()


public:
	AC_MedKit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	void HandleActivatingState() override;
	void InitStartVariables() override;

};