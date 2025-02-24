// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvenUI/Panel/C_BasicPanelWidget.h"
#include "Components/ListView.h"

#include "C_BasicItemPanelWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BasicItemPanelWidget : public UC_BasicPanelWidget
{
	GENERATED_BODY()
	
public:
	
   //void NativeConstruct();
   //
   //void OnEntryInitialized(UUserWidget& EntryWidget, AC_Item* ListItem);
    
    /// <summary>
    /// MyItemListWidget(UI에 보이는 Inventory의 itemlist)를 초기화 하는 함수.
    /// </summary>
    /// <param name="itemlist"></param>
    //UFUNCTION(BlueprintCallable)
    void AddTMapItem(TMap<FString, TArray<AC_Item*>> MyItemMap);

    void UpdateMyItemList(TMap<FString, AC_Item*> MyItemMap);

    /// <summary>
    /// AroundItemListWidget를 초기화 하는 함수.
    /// </summary>
    /// <param name="AroundItemList"></param>
    UFUNCTION(BlueprintCallable)
    void InitializeItemList(const TArray<AC_Item*>& AroundItemList);

public:
    UListView* GetItemListView() { return ItemListView1; }
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
    UListView* ItemListView1 = nullptr;
};


