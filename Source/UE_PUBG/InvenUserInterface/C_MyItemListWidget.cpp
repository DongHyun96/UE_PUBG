// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUserInterface/C_MyItemListWidget.h"
#include "InvenUserInterface/C_ItemBarWidget.h"

#include "Item/C_Item.h"
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

    ItemListView->ClearListItems(); // 기존 아이템 삭제

    //if (itemList)

    if (!(MyItemlist.Num() > 0)) return;
    for (const auto& ItemPair : MyItemlist)
    {
        AC_Item* Item = ItemPair.Value; // TMap에서 아이템 가져오기
        if (IsValid(Item))
        {
            ItemListView->AddItem(Item);
        }
        else
        {
            return;
        }
        //ItemBar갱신.
        UC_ItemBarWidget* EntryWidget = Cast<UC_ItemBarWidget>(ItemListView->GetEntryWidgetFromItem(Item));
        //UC_ItemBarWidget* EntryWidget = Cast<UC_ItemBarWidget>(MyItemListWidget->ItemListBar->GetEntryWidgetFromItem(Item));

        if (IsValid(EntryWidget))
            EntryWidget->InitBar(Item);
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

