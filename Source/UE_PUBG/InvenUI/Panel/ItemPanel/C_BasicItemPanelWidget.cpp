// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/Panel/ItemPanel/C_BasicItemPanelWidget.h"
#include "InvenUI/ItemBar/C_BasicItemBarWidget.h"
#include "Item/C_Item.h"
#include "Item/ConsumableItem/C_ConsumableItem.h"

void UC_BasicItemPanelWidget::AddTMapItem(TMap<FString, TArray<AC_Item*>> MyItemMap)
{
    if (!IsValid(ItemListView1)) return;
    
    ItemListView1->ClearListItems(); // 기존 아이템 삭제

    if (MyItemMap.Num() == 0) return;

    for (const auto& ItemPairTArray : MyItemMap)
    {
        TArray<AC_Item*> ItemTArray = ItemPairTArray.Value; // TMap에서 아이템 가져오기

        for (const auto& Item : ItemTArray)
        {
            if (IsValid(Item))
            {
                ItemListView1->AddItem(Item);
            }
            else
            {
                continue;
            }

            //ItemBar갱신.
            UC_BasicItemBarWidget* EntryWidget = Cast<UC_BasicItemBarWidget>(ItemListView1->GetEntryWidgetFromItem(Item));
            //UC_ItemBarWidget* EntryWidget = Cast<UC_ItemBarWidget>(MyItemListWidget->ItemListBar->GetEntryWidgetFromItem(Item));

            if (IsValid(EntryWidget))
            {
                if (AC_ConsumableItem* ConsumableItem = Cast<AC_ConsumableItem>(Item))
                {
                    if (IsValid(ConsumableItem->GetTestLinkedItemBarWidget()))
                    {
                        EntryWidget = ConsumableItem->GetTestLinkedItemBarWidget();
                    }
                    else
                    {
                        ConsumableItem->SetLinkedItemBarWidget(EntryWidget);
                    }
                }
                EntryWidget->UpdateWidget(Item);
            }
        }
    }
}

void UC_BasicItemPanelWidget::UpdateMyItemList(TMap<FString, AC_Item*> MyItemMap)
{
    if (!IsValid(ItemListView1)) return;

    ItemListView1->ClearListItems(); // 기존 아이템 삭제

    if (!(MyItemMap.Num() > 0)) return;

    for (const auto& MyItem: MyItemMap)
    {
        AC_Item* Item = MyItem.Value;

        if (IsValid(Item))
        {
            ItemListView1->AddItem(Item);
        }
        else
        {
            continue;
        }

        //ItemBar갱신.
        UC_BasicItemBarWidget* EntryWidget = Cast<UC_BasicItemBarWidget>(ItemListView1->GetEntryWidgetFromItem(Item));
        //UC_ItemBarWidget* EntryWidget = Cast<UC_ItemBarWidget>(MyItemListWidget->ItemListBar->GetEntryWidgetFromItem(Item));

        if (IsValid(EntryWidget))
        {
            if (AC_ConsumableItem* ConsumableItem = Cast<AC_ConsumableItem>(Item))
            {
                if (IsValid(ConsumableItem->GetTestLinkedItemBarWidget()))
                {
                    EntryWidget = ConsumableItem->GetTestLinkedItemBarWidget();
                }
                else
                {
                    ConsumableItem->SetLinkedItemBarWidget(EntryWidget);
                }
            }
            EntryWidget->UpdateWidget(Item);
        }
    }
}

void UC_BasicItemPanelWidget::InitializeItemList(const TArray<AC_Item*>& AroundItemList)
{
    if (!IsValid(ItemListView1)) return;

    ItemListView1->ClearListItems(); // 기존 아이템 삭제

    if (!(AroundItemList.Num() > 0)) return;

    for (auto& AroundItem : AroundItemList)
    {
        AC_Item* Item = AroundItem;
        if (IsValid(Item))
        {
            ItemListView1->AddItem(Item);
        }
        else
        {
            return;
        }

        UC_BasicItemBarWidget* EntryWidget = Cast<UC_BasicItemBarWidget>(ItemListView1->GetEntryWidgetFromItem(Item));

        if (EntryWidget)
            EntryWidget->UpdateWidget(Item);
    }
}
