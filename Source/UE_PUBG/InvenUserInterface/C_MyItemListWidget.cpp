// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUserInterface/C_MyItemListWidget.h"
#include "InvenUserInterface/C_ItemBarWidget.h"

#include "Item/C_Item.h"
#include "Item/ConsumableItem/C_ConsumableItem.h"

#include "Utility/C_Util.h"
//#include "Kismet/KismetRenderingLibrary.h" // For widget creation

void UC_MyItemListWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (!ItemListView)
    {
        UC_Util::Print("No ItemListView");
    }
}

void UC_MyItemListWidget::AddTMapItem(TMap<FString, TArray<AC_Item*>> MyItemlist)
{
    if (!IsValid(ItemListView))
    {
        UC_Util::Print("void List");
        return;
    }

    ItemListView->ClearListItems(); // 기존 아이템 삭제

    //if (itemList)

    if (!(MyItemlist.Num() > 0)) return;
    for (const auto& ItemPairTArray : MyItemlist)
    {
        TArray<AC_Item*> ItemTArray = ItemPairTArray.Value; // TMap에서 아이템 가져오기

        for (const auto& Item : ItemTArray)
        {
            if (IsValid(Item))
            {
                ItemListView->AddItem(Item);
            }
            else
            {
                continue;
            }

            //ItemBar갱신.
            UC_ItemBarWidget* EntryWidget = Cast<UC_ItemBarWidget>(ItemListView->GetEntryWidgetFromItem(Item));
            //UC_ItemBarWidget* EntryWidget = Cast<UC_ItemBarWidget>(MyItemListWidget->ItemListBar->GetEntryWidgetFromItem(Item));

            if (IsValid(EntryWidget))
            {
                if (AC_ConsumableItem* ConsumableItem = Cast<AC_ConsumableItem>(Item))
                {
                    if (IsValid(ConsumableItem->GetLinkedItemBarWidget()))
                    {
                        EntryWidget = ConsumableItem->GetLinkedItemBarWidget();
                    }
                    else
                    {
                        ConsumableItem->SetLinkedItemBarWidget(EntryWidget);
                    }
                }
                EntryWidget->InitBar(Item);
            }
        }
    }
}

void UC_MyItemListWidget::InitAroundItemList(const TArray<AC_Item*>& itemList)
{
    if (!IsValid(ItemListView))
    {
        UC_Util::Print("void List");
        return;
    }

    ItemListView->ClearListItems(); // 기존 아이템 삭제

    if (!(itemList.Num() > 0)) return;

    for (auto& AroundItem : itemList)
    {
        AC_Item* Item = AroundItem;
        if (IsValid(Item))
        {
            ItemListView->AddItem(Item);
        }
        else
        {
            return;
        }
        //UC_ItemBarWidget* EntryWidget = Cast<UC_ItemBarWidget>(MyItemListView->GetEntryWidgetFromItem(Item));
        UC_ItemBarWidget* EntryWidget = Cast<UC_ItemBarWidget>(ItemListView->GetEntryWidgetFromItem(Item));

        if (EntryWidget)
            EntryWidget->InitBar(Item);
    }
}

void UC_MyItemListWidget::InitMyItemList(const TMap<FString, AC_Item*> MyItemlist)
{
}

