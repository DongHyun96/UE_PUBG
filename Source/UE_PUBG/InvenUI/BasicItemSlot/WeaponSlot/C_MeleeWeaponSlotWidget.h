﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/C_WeaponSlotWidget.h"
#include "C_MeleeWeaponSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_MeleeWeaponSlotWidget : public UC_WeaponSlotWidget
{
	GENERATED_BODY()
public:
	//virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	//void UpdateWidget() override;

	bool MouseRBDownInteraction(AC_Weapon* inSlotWeapon) override;
};


