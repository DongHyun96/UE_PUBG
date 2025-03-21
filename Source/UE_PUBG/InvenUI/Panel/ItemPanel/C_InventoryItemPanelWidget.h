// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvenUI/Panel/ItemPanel/C_BasicItemPanelWidget.h"
#include "C_InventoryItemPanelWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_InventoryItemPanelWidget : public UC_BasicItemPanelWidget
{
	GENERATED_BODY()

protected:
	bool HandleDrop(class UC_ItemBox* DroppedItemBox) override;

	void ShowDividePanelWidget(UC_ItemBox* DividedItemBox) override;

};


