// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvenUI/C_CustomUserWidget.h"
#include "C_BasicPanelWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BasicPanelWidget : public UC_CustomUserWidget
{
	GENERATED_BODY()
public:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

protected:
	virtual bool HandleDrop(class AC_Item* DroppedItem); //TODO: 순수가상함수로

	virtual void ShowDividePanelWidget(AC_Item* DroppedItem);
};


