﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvenUI/C_CustomUserWidget.h"
#include "C_BasicEquippedItemSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BasicEquippedItemSlotWidget : public UC_CustomUserWidget
{
	GENERATED_BODY()
public:
	//virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual void UpdateWidget();
};


