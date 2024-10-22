// Fill out your copyright notice in the Description page of Project Settings.

#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/ListView.h"
#include "UE_PUBG/Character/C_BasicCharacter.h"
#include "C_MyItemListWidget.h"

#include "Components/CanvasPanel.h"

#include "InvenUserInterface/C_InvenUiWidget.h"
#include "InvenUserInterface/C_ItemBarWidget.h"

#include "Utility/C_Util.h"

void UC_InvenUiWidget::NativeConstruct()
{
    Super::NativeConstruct();

    //InvenCanvas = Cast<UCanvasPanel>(GetWidgetFromName(FName("InvenCanvas1")));
    
    if (InvenCanvas)
    {
        
    }

    MyItemListWidget = Cast<UC_MyItemListWidget>(GetWidgetFromName(FName("MyItemListWidget1")));
    AroundItemListWidget = Cast<UC_MyItemListWidget>(GetWidgetFromName(FName("AroundItemListWidget1")));

    MyItemListView = Cast<UListView>(GetWidgetFromName(FName("MyItemList")));
    AroundItemListView = Cast<UListView>(GetWidgetFromName(FName("AroundItemList")));

    //MyItemListView->ItemFromEntryWidget

    if (ItemBarClass)
    {
        ItemBarInstance = CreateWidget<UC_ItemBarWidget>(GetWorld(), ItemBarClass);
    }

    InitListView();

    // 아이템 리스트 위젯 초기화 및 데이터 추가
    if (MyItemListWidget)
    {
        TMap<FString, AC_Item*> MyItems; // 실제 아이템 리스트를 가져오는 로직 필요
        MyItems = OwnerCharacter->GetInvenComponent()->GetTestMyItems();
        MyItemListWidget->SetVisibility(ESlateVisibility::Visible);

        MyItemListWidget->AddTMapItem(MyItems); // 아이템 리스트 추가
        MyItemListWidget->AddToViewport();
    }

    if (AroundItemListWidget)
    {
        TMap<FString, AC_Item*> AroundItems; // 실제 아이템 리스트를 가져오는 로직 필요
        AroundItems = OwnerCharacter->GetInvenComponent()->GetTestAroundItems();
        AroundItemListWidget->SetVisibility(ESlateVisibility::Visible);

        AroundItemListWidget->AddTMapItem(AroundItems);
        AroundItemListWidget->AddToViewport();
    }

    // 아이템 리스트 위젯 초기화 및 데이터 추가
    //if (MyItemListView)
    //{
    //    //MyItemListView->AddToViewport();
    //    TMap<FString, AC_Item*> MyItems; // 실제 아이템 리스트를 가져오는 로직 필요
    //    MyItems = OwnerCharacter->GetInvenComponent()->GetTestMyItems();

    //    MyItemListView->AddItem(MyItems);
    //}

    //if (AroundItemListWidget)
    //{
    //    AroundItemListWidget->AddToViewport();
    //    TMap<FString, AC_Item*> AroundItems; // 실제 아이템 리스트를 가져오는 로직 필요
    //    AroundItems = OwnerCharacter->GetInvenComponent()->GetTestAroundItems();

    //    AroundItemListWidget->AddTMapItem(AroundItems);
    //}
}

void UC_InvenUiWidget::InitListView()
{
    //     아이템 리스트 위젯 초기화 및 데이터 추가
    if (MyItemListView)
    { 
        //MyItemListView->AddToViewport();
        TMap<FString, AC_Item*> MyItems; // 실제 아이템 리스트를 가져오는 로직 필요
        MyItems = OwnerCharacter->GetInvenComponent()->GetTestMyItems();

        PopulateItemList(MyItemListView, MyItems);
    }

    if (AroundItemListView)
    {
        //AroundItemListView->AddToViewport();
        TMap<FString, AC_Item*> AroundItems; // 실제 아이템 리스트를 가져오는 로직 필요
        AroundItems = OwnerCharacter->GetInvenComponent()->GetTestAroundItems();

        PopulateItemList(AroundItemListView, AroundItems);

    }
}

void UC_InvenUiWidget::PopulateItemList(UListView* list, const TMap<FString, AC_Item*>& itemList)
{
    list->ClearListItems(); // 기존 아이템 삭제

    
    for (const auto& ItemPair : itemList)
    {
        AC_Item* Item = ItemPair.Value; // TMap에서 아이템 가져오기
        if (Item)
        {
            // UC_ItemBarWidget 인스턴스 생성
            //UC_ItemBarWidget* NewItemBarWidget = CreateWidget<UC_ItemBarWidget>(GetWorld(), UC_ItemBarWidget::StaticClass());
            list->GetEntryWidgetClass();
            list->AddItem(Item);
            //UC_ItemBarWidget* NewItemBarWidget = Cast<UC_ItemBarWidget>(GetWidgetFromName(FName("WBP_ItemBarWidget")));
            //if (ItemBarInstance)
            //{
            //    // 아이템 데이터 초기화
            //    ItemBarInstance->InitBar(Item); // InitBar 메서드를 통해 아이템 정보를 설정
            //    list->AddItem(ItemBarInstance); // 리스트뷰에 아이템 추가
            //}
        }
    }
}
