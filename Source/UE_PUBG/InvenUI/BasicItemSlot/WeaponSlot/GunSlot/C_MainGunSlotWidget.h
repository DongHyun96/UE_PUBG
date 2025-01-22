// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_GunSlotWidget.h"
#include "C_MainGunSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_MainGunSlotWidget : public UC_GunSlotWidget
{
	GENERATED_BODY()

public:
	//virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	//virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	//void UpdateWidget() override;
protected:
	//bool HandleDrop(class AC_Item* DroppedItem) override;

	bool ChangedGunSlot(class AC_Gun* gun) override;
};
