// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUserInterface/C_MyItemListWidget.h"
#include "InvenUserInterface/C_ItemBarWidget.h"

#include "Item/C_Item.h"
//#include "Kismet/KismetRenderingLibrary.h" // For widget creation

void UC_MyItemListWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (ItemListBar)
    {
        // ItemClicked ��������Ʈ�� ���ε�
        //ItemListBar->EntryWidgetClass = UC_ItemBarWidget::StaticClass();
        
    }
    
    ItemListBar = Cast<UListView>(GetWidgetFromName(FName("AroundItemList")));
}

void UC_MyItemListWidget::AddTMapItem(TMap<FString, AC_Item*> itemlist)
{
    ItemListBar = Cast<UListView>(GetWidgetFromName(FName("AroundItemList")));
    ItemListBar = Cast<UListView>(GetWidgetFromName(FName("ItemListBar1")));

    PopulateItemList(itemlist); // TMap ������ ����Ʈ�� ����Ʈ�信 �߰�
}

void UC_MyItemListWidget::PopulateItemList(const TMap<FString, AC_Item*>& itemList)
{
    ItemListBar = Cast<UListView>(GetWidgetFromName(FName("AroundItemList")));

    if (!IsValid(ItemListBar))
    {
        return;
    }

    ItemListBar->ClearListItems(); // ���� ������ ����


    for (const auto& ItemPair : itemList)
    {
        AC_Item* Item = ItemPair.Value; // TMap���� ������ ��������
        if (Item)
        {
            // UC_ItemBarWidget �ν��Ͻ� ����
            UC_ItemBarWidget* NewItemBarWidget = CreateWidget<UC_ItemBarWidget>(GetWorld(), UC_ItemBarWidget::StaticClass());
            if (NewItemBarWidget)
            {
                // ������ ������ �ʱ�ȭ
                NewItemBarWidget->InitBar(Item); // InitBar �޼��带 ���� ������ ������ ����
                ItemListBar->AddItem(NewItemBarWidget); // ����Ʈ�信 ������ �߰�
            }

            //if (NewItemBarWidget)
            //{
            //    // ������ ������ �ʱ�ȭ
            //    NewItemBarWidget->InitBar(Item); // InitBar �޼��带 ���� ������ ������ ����
            //    ItemListBar->AddItem(NewItemBarWidget); // ����Ʈ�信 ������ �߰�
            //}
        }
    }
}

