// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUserInterface/C_ItemListWidget.h"
#include "Components/ListView.h"

void UC_ItemListWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ItemListBar)
    {
        // ItemClicked 델리게이트에 바인딩
    }
}

void UC_ItemListWidget::AddTMapItem(TMap<FString, AC_Item*> itemlist)
{
    for (const TPair<FString, AC_Item*>& Pair : itemlist)
    {
        AC_Item* Item = Pair.Value;
        if (Item)
        {
            ItemListBar->AddItem(Item);
        }
    }
    ItemListBar->RequestRefresh();

}

