// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/C_Item.h"
#include "C_EquipableItem.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_EquipableItem : public AC_Item
{
	GENERATED_BODY()
	
public:
	AC_EquipableItem();
	
	void Interaction(class AC_BasicCharacter* character) override;

};
