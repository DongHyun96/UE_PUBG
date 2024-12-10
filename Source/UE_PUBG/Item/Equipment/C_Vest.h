// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "C_Vest.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_Vest : public AC_EquipableItem
{
	GENERATED_BODY()
	
public:
	AC_Vest();

	bool Interaction(AC_BasicCharacter* Character) override;

	void AttachToSocket(class AC_BasicCharacter* InParent) override;
protected:
	float Durability = 0;

};
