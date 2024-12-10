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

void UC_MyItemListWidget::AddTMapItem(TMap<FString, AC_Item*> MyItemlist)
{
    if (!IsValid(ItemListView))
    {
        UC_Util::Print("void List");
        return;
    }

    //ItemListView->ClearListItems(); // ���� ������ ����

    //if (itemList)

    if (!(MyItemlist.Num() > 0)) return;
    for (const auto& ItemPair : MyItemlist)
    {
        AC_Item* Item = ItemPair.Value; // TMap���� ������ ��������

        //if (AC_ConsumableItem* ConsumableItem = Cast<AC_ConsumableItem>(Item))
        //    if (ConsumableItem->GetLinkedItemBarWidget()) continue;

        if (IsValid(Item))
        {
            ItemListView->AddItem(Item);
        }
        else
        {
            continue;
        }



        //ItemBar����.
        UC_ItemBarWidget* EntryWidget = Cast<UC_ItemBarWidget>(ItemListView->GetEntryWidgetFromItem(Item));
        //UC_ItemBarWidget* EntryWidget = Cast<UC_ItemBarWidget>(MyItemListWidget->ItemListBar->GetEntryWidgetFromItem(Item));

        if (IsValid(EntryWidget))
        {
            if (AC_ConsumableItem* ConsumableItem = Cast<AC_ConsumableItem>(Item))
            {
                ConsumableItem->SetLinkedItemBarWidget(EntryWidget);
            }
            

            EntryWidget->InitBar(Item);
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

    ItemListView->ClearListItems(); // ���� ������ ����

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

