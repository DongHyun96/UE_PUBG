// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemManager/C_ItemManager.h"
#include "Utility/C_Util.h"

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
    static ConstructorHelpers::FObjectFinder<UDataTable> GeneralItemDataTableObject(TEXT("/Game/Project_PUBG/Common/Item/ItemDataTables/DT_Item.DT_Item"));
    static ConstructorHelpers::FObjectFinder<UDataTable>     GunItemDataTableObject(TEXT("/Game/Project_PUBG/Common/Item/ItemDataTables/DT_GunData.DT_GunData"));

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

        for (const FName& RowName : RowNames)
        {
            FGunData* GunItemData = GunItemTable->FindRow<FGunData>(RowName, TEXT("LoadGunDataTable"));

            if (GunItemData)
            {
                GunItemDataCache.Add(RowName, *GunItemData);
            }
        }
    }
}

AC_Item* UC_ItemManager::SpawnItem(FName ItemCode, FVector Location, int32 Stack)
{
    UC_Util::Print(GeneralItemDataCache.Num());
    if (GeneralItemDataCache.Num() == 0) return nullptr;

    if (!GeneralItemDataCache.Contains(ItemCode)) return nullptr;

    //const FItemData* FoundData = ItemDataTable->FindRow<FItemData>(ItemCode, TEXT("SpawnItem"));
    //if (!FoundData || !FoundData->ItemClass) return nullptr;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AC_Item* NewItem = GetWorld()->SpawnActor<AC_Item>(
        GeneralItemDataCache[ItemCode].ItemClass, Location, FRotator::ZeroRotator, SpawnParams
    );

    if (NewItem)
    {
        if (Stack != 0)
            NewItem->SetItemStack(Stack); // 필요한 초기값 설정
    }

    return NewItem;
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
