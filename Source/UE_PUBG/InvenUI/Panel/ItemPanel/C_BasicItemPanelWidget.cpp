// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/Panel/ItemPanel/C_BasicItemPanelWidget.h"
#include "InvenUI/ItemBar/C_BasicItemBarWidget.h"
#include "Item/C_Item.h"
#include "Item/ConsumableItem/C_ConsumableItem.h"
#include "Utility/C_Util.h"

void UC_BasicItemPanelWidget::UpdateInventoryItemList(TMap<FName, TArray<AC_Item*>> MyItemMap)
{
    if (!IsValid(ItemListView1)) return;

    // 아이템 리스트 초기화
    ItemListView1->ClearListItems();
    if (MyItemMap.Num() == 0) return;

    // 추가할 아이템 목록
    TArray<AC_Item*> ItemsToAdd;

    // MyItemMap을 순회하면서 모든 아이템을 리스트에 추가
    for (const auto& MyItem : MyItemMap)
    {
        for (AC_Item* Item : MyItem.Value) // 배열 안의 모든 아이템 처리
        {
            if (IsValid(Item))
            {
                ItemsToAdd.Emplace(Item);
            }
        }
    }
    //아이템 타입(EItemType) 기준 정렬
    ItemsToAdd.Sort([](const AC_Item& A, const AC_Item& B)
        {
            return A.GetItemDatas()->ItemType < B.GetItemDatas()->ItemType;
        });


    //ListView에 아이템 추가
    if (ItemsToAdd.Num() > 0)
    {
        ItemListView1->SetListItems(ItemsToAdd); //여기서 먼저 추가

        //ConsumableItem의 LinkedItemBarWidget 설정 
        for (AC_Item* Item : ItemsToAdd)
        {
            if (AC_ConsumableItem* ConsumableItem = Cast<AC_ConsumableItem>(Item))
            {
                // ListView에서 해당 아이템에 대한 EntryWidget 가져오기 (이제 유효할 가능성이 높음)
                UC_BasicItemBarWidget* EntryWidget = Cast<UC_BasicItemBarWidget>(ItemListView1->GetEntryWidgetFromItem(Item));

                if (!EntryWidget)
                    ConsumableItem->SetLinkedItemBarWidget(EntryWidget);

                //if (IsValid(ConsumableItem->GetLinkedItemBarWidget()))
                //{
                //    EntryWidget = ConsumableItem->GetLinkedItemBarWidget();
                //}
                //else
                //{
                //}
                //EntryWidget->UpdateWidget(Item);
            }
            UC_Util::Print(Item->GetItemCurStack());
        }
    }
}

void UC_BasicItemPanelWidget::UpdateAroundItemList(const TArray<AC_Item*>& AroundItemList)
{
    if (!IsValid(ItemListView1)) return;

    ItemListView1->ClearListItems(); // 기존 아이템 삭제
    //ItemListView1->SetListItems
    if (!(AroundItemList.Num() > 0)) return;

    TArray<AC_Item*> SortedItems = AroundItemList;

    //오름차순 정렬
    //SortedItems.Sort([](AC_Item* A, AC_Item* B) 
    //    {
    //        if (!A && !B) return false;
    //        if (!A) return false;
    //        if (!B) return true;
    //        return A->GetItemDatas()->ItemType < B->GetItemDatas()->ItemType;
    //    });


    // 한 번에 아이템을 추가
    if (SortedItems.Num() > 0)
    {
        ItemListView1->SetListItems(SortedItems);
    }

}

void UC_BasicItemPanelWidget::RemoveItemInList(AC_Item* InItem)
{
    ItemListView1->RemoveItem(InItem);
}



