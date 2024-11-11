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
        // ItemClicked 델리게이트에 바인딩
        //ItemListBar->EntryWidgetClass = UC_ItemBarWidget::StaticClass();
        
    }
    
    ItemListBar = Cast<UListView>(GetWidgetFromName(FName("AroundItemList")));
}

void UC_MyItemListWidget::AddTMapItem(TMap<FString, AC_Item*> itemlist)
{
    ItemListBar = Cast<UListView>(GetWidgetFromName(FName("AroundItemList")));
    ItemListBar = Cast<UListView>(GetWidgetFromName(FName("ItemListBar1")));

    PopulateItemList(itemlist); // TMap 아이템 리스트를 리스트뷰에 추가
}

void UC_MyItemListWidget::PopulateItemList(const TMap<FString, AC_Item*>& itemList)
{
    ItemListBar = Cast<UListView>(GetWidgetFromName(FName("AroundItemList")));

    if (!IsValid(ItemListBar))
    {
        return;
    }

    ItemListBar->ClearListItems(); // 기존 아이템 삭제


    for (const auto& ItemPair : itemList)
    {
        AC_Item* Item = ItemPair.Value; // TMap에서 아이템 가져오기
        if (Item)
        {
            // UC_ItemBarWidget 인스턴스 생성
            UC_ItemBarWidget* NewItemBarWidget = CreateWidget<UC_ItemBarWidget>(GetWorld(), UC_ItemBarWidget::StaticClass());
            if (NewItemBarWidget)
            {
                // 아이템 데이터 초기화
                NewItemBarWidget->InitBar(Item); // InitBar 메서드를 통해 아이템 정보를 설정
                ItemListBar->AddItem(NewItemBarWidget); // 리스트뷰에 아이템 추가
            }

            //if (NewItemBarWidget)
            //{
            //    // 아이템 데이터 초기화
            //    NewItemBarWidget->InitBar(Item); // InitBar 메서드를 통해 아이템 정보를 설정
            //    ItemListBar->AddItem(NewItemBarWidget); // 리스트뷰에 아이템 추가
            //}
        }
    }
}

