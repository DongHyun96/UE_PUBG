// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemManager/C_ItemManager.h"

UC_ItemManager::UC_ItemManager()
{
}

void UC_ItemManager::InitializeItemManager()
{
    LoadItemDataTable();
}

void UC_ItemManager::LoadItemDataTable()
{
    static ConstructorHelpers::FObjectFinder<UDataTable> ItemDataTableObject(TEXT("/Game/Project_PUBG/Common/Item/DB_Item.DB_Item"));

    if (ItemDataTableObject.Succeeded())
    {
        GeneralItemTable = ItemDataTableObject.Object;

        TArray<FName> RowNames = GeneralItemTable->GetRowNames();

        //데이터 테이블에서 모든 아이템을 캐싱
        for (const FName& RowName : RowNames)
        {
            FItemData* ItemData = GeneralItemTable->FindRow<FItemData>(RowName, TEXT("LoadItemDataTable"));

            if (ItemData)
            {
                ItemDataCache.Add(RowName, *ItemData);
            }
        }
    }
}

FItemData* UC_ItemManager::GetItemData(FName ItemCode)
{
    if (ItemDataCache.Contains(ItemCode))
    {
        return &ItemDataCache[ItemCode];
    }
    return nullptr;
}
