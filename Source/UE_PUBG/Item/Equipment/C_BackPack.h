// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "C_BackPack.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_BackPack : public AC_EquipableItem
{
	GENERATED_BODY()
	
public:
	AC_BackPack();

	void Interaction(class AC_BasicCharacter* character) override;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 Level;
};
