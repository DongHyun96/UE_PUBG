// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvenUI/C_CustomUserWidget.h"
#include "C_BasicDivideItemPanelWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BasicDivideItemPanelWidget : public UC_CustomUserWidget
{
	GENERATED_BODY()
public:
	void SetCachedItem(class UC_ItemBox* InteractionItemBox) { CachedItemBox = InteractionItemBox; }

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateWidget();
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UC_ItemBox* CachedItemBox = nullptr;
};
