// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvenUI/Panel/C_BasicPanelWidget.h"
#include "Components/ListView.h"
#include "Item/C_ItemBox.h"
#include "C_BasicItemPanelWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BasicItemPanelWidget : public UC_BasicPanelWidget
{
	GENERATED_BODY()
	
public:
	
    /// <summary>
    /// InventoryItemList의 Update함수
    /// </summary>
    /// <param name="MyItemMap"></param>
    void UpdateInventoryItemList(TMap<FName, AC_Item*> MyItemMap);

    /// <summary>
    /// AroundItemList의 Update함수.
    /// </summary>
    /// <param name="AroundItemList"></param>
    UFUNCTION(BlueprintCallable)
    void UpdateAroundItemList(const TArray<AC_Item*>& AroundItemList);

    void RemoveItemInList(AC_Item* InItem);

public:
    UListView* GetItemListView() { return ItemListView1; }
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
    UListView* ItemListView1 = nullptr;


};


