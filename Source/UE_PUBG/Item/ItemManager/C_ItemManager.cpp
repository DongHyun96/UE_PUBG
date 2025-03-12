// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemManager/C_ItemManager.h"

UC_ItemManager::UC_ItemManager()
{
    InitializeItemManager();
}

void UC_ItemManager::InitializeItemManager()
{
    LoadItemDataTable();
}

void UC_ItemManager::LoadItemDataTable()
{
    static ConstructorHelpers::FObjectFinder<UDataTable> GeneralItemDataTableObject(TEXT("/Game/Project_PUBG/Common/Item/DB_Item.DB_Item"));
    static ConstructorHelpers::FObjectFinder<UDataTable>     GunItemDataTableObject(TEXT("/Game/Project_PUBG/Common/Item/DT_GunData.DT_GunData"));

    if (GeneralItemDataTableObject.Succeeded())
    {
        GeneralItemTable = GeneralItemDataTableObject.Object;

        TArray<FName> RowNames = GeneralItemTable->GetRowNames();

        //데이터 테이블에서 모든 아이템을 캐싱
        for (const FName& RowName : RowNames)
        {
            FItemData* ItemData = GeneralItemTable->FindRow<FItemData>(RowName, TEXT("LoadItemDataTable"));

            if (ItemData)
            {
                GeneralItemDataCache.Add(RowName, *ItemData);
            }
        }
    }

    if (GunItemDataTableObject.Succeeded())
    {
        GunItemTable = GunItemDataTableObject.Object;

        TArray<FName> RowNames = GunItemTable->GetRowNames();

        //데이터 테이블에서 모든 아이템을 캐싱
        for (const FName& RowName : RowNames)
        {
            FGunData* GunItemData = GunItemTable->FindRow<FGunData>(RowName, TEXT("LoadItemDataTable"));

            if (GunItemData)
            {
                GunItemDataCache.Add(RowName, *GunItemData);
            }
        }
    }
}

FItemData* UC_ItemManager::GetItemData(FName ItemCode)
{
    if (GeneralItemDataCache.Contains(ItemCode))
    {
        return &GeneralItemDataCache[ItemCode];
    }
    return nullptr;
}

FGunData* UC_ItemManager::GetGunData(FName ItemCode)
{
    if (GunItemDataCache.Contains(ItemCode))
    {
        return &GunItemDataCache[ItemCode];
    }
    return nullptr;
}
