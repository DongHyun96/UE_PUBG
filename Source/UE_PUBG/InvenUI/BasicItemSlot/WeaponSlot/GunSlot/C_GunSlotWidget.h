// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/C_WeaponSlotWidget.h"
#include "C_GunSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_GunSlotWidget : public UC_WeaponSlotWidget
{
	GENERATED_BODY()

public:
	bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	bool MouseRBDownInteraction(AC_Weapon* inSlotWeapon) override;
protected:
	virtual bool HandleDrop(class AC_Item* DroppedItem); //TODO: 순수가상함수로

	virtual bool ChangedGunSlot(AC_Weapon* gun);
};
