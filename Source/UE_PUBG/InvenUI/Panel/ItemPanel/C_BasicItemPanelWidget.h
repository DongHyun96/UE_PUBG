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

	void NativeConstruct() override;

    /// <summary>
    /// InventoryItemList의 Update함수
    /// </summary>
    /// <param name="MyItemMap"></param>
    void UpdateInventoryItemList(TMap<FName, TArray<AC_Item*>> MyItemMap);

    /// <summary>
    /// AroundItemList의 Update함수.
    /// </summary>
    /// <param name="AroundItemList"></param>
    UFUNCTION(BlueprintCallable)
    void UpdateAroundItemList(const TArray<AC_Item*>& AroundItemList);

	void AddItemToInventoryItemList(AC_Item* InItem);

	void AddItemToAroundItemList(AC_Item* InItem);

	void AddItemToList(AC_Item* InItem);

    void RemoveItemInList(AC_Item* InItem);
    
	void RemoveItemInList(class UC_ItemDataObject* InDataObj);

    void PreInitializeItemBars();

public:
    UListView* GetItemListView() { return ItemListView; }
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
    UListView* ItemListView = nullptr;

    // DataObject 풀
    UPROPERTY()
    TArray<UC_ItemDataObject*> DataObjectPool{};

    // 화면에 현재 추가된 객체들
    UPROPERTY()
    TArray<UC_ItemDataObject*> ActiveDataObjects{};

    // 풀 크기
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemPanel")
    int32 MaxPoolSize = 30;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> ItemBarWidgetClass;

protected:
    // DataObject 생성 함수
    UC_ItemDataObject* GetDataObject();

    void ReturnDataObject(UC_ItemDataObject* Obj);
};


