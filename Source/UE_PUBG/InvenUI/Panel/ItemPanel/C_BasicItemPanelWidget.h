// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvenUI/Panel/C_BasicPanelWidget.h"
#include "Components/ListView.h"

#include "C_BasicItemPanelWidget.generated.h"

struct FInventoryItemBox :public UObject
{
    AC_Item* ItemRef;  // 실제 AC_Item 객체 참조 (새로 만들지 않음)
    int32 StackCount;  // UI에서 보여줄 개수

    FInventoryItemBox(AC_Item* InItem, int32 InStackCount)
        : ItemRef(InItem), StackCount(InStackCount) {
    }
};

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


