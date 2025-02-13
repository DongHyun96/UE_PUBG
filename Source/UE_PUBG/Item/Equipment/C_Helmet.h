// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "C_Helmet.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_Helmet : public AC_EquipableItem
{
	GENERATED_BODY()

public:

	bool TakeDamage(float DamageAmount) override;

	float GetDamageReduceFactor() const override;
};


