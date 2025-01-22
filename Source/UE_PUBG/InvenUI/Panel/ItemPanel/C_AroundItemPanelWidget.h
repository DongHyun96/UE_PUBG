// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvenUI/Panel/ItemPanel/C_BasicItemPanelWidget.h"
#include "C_AroundItemPanelWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_AroundItemPanelWidget : public UC_BasicItemPanelWidget
{
	GENERATED_BODY()

protected:
	bool HandleDrop(class AC_Item* DroppedItem) override;
	
};
