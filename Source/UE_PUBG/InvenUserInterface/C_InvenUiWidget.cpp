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

    // ������ ����Ʈ ���� �ʱ�ȭ �� ������ �߰�
    if (MyItemListWidget)
    {
        TMap<FString, AC_Item*> MyItems; // ���� ������ ����Ʈ�� �������� ���� �ʿ�
        MyItems = OwnerCharacter->GetInvenComponent()->GetTestMyItems();
        MyItemListWidget->SetVisibility(ESlateVisibility::Visible);

        MyItemListWidget->AddTMapItem(MyItems); // ������ ����Ʈ �߰�
        MyItemListWidget->AddToViewport();
    }

    if (AroundItemListWidget)
    {
        TMap<FString, AC_Item*> AroundItems; // ���� ������ ����Ʈ�� �������� ���� �ʿ�
        AroundItems = OwnerCharacter->GetInvenComponent()->GetTestAroundItems();
        AroundItemListWidget->SetVisibility(ESlateVisibility::Visible);

        AroundItemListWidget->AddTMapItem(AroundItems);
        AroundItemListWidget->AddToViewport();
    }

    // ������ ����Ʈ ���� �ʱ�ȭ �� ������ �߰�
    //if (MyItemListView)
    //{
    //    //MyItemListView->AddToViewport();
    //    TMap<FString, AC_Item*> MyItems; // ���� ������ ����Ʈ�� �������� ���� �ʿ�
    //    MyItems = OwnerCharacter->GetInvenComponent()->GetTestMyItems();

    //    MyItemListView->AddItem(MyItems);
    //}

    //if (AroundItemListWidget)
    //{
    //    AroundItemListWidget->AddToViewport();
    //    TMap<FString, AC_Item*> AroundItems; // ���� ������ ����Ʈ�� �������� ���� �ʿ�
    //    AroundItems = OwnerCharacter->GetInvenComponent()->GetTestAroundItems();

    //    AroundItemListWidget->AddTMapItem(AroundItems);
    //}
}

void UC_InvenUiWidget::InitListView()
{
    //     ������ ����Ʈ ���� �ʱ�ȭ �� ������ �߰�
    if (MyItemListView)
    { 
        //MyItemListView->AddToViewport();
        TMap<FString, AC_Item*> MyItems; // ���� ������ ����Ʈ�� �������� ���� �ʿ�
        MyItems = OwnerCharacter->GetInvenComponent()->GetTestMyItems();

        PopulateItemList(MyItemListView, MyItems);
    }

    if (AroundItemListView)
    {
        //AroundItemListView->AddToViewport();
        TMap<FString, AC_Item*> AroundItems; // ���� ������ ����Ʈ�� �������� ���� �ʿ�
        AroundItems = OwnerCharacter->GetInvenComponent()->GetTestAroundItems();

        PopulateItemList(AroundItemListView, AroundItems);

    }
}

void UC_InvenUiWidget::PopulateItemList(UListView* list, const TMap<FString, AC_Item*>& itemList)
{
    list->ClearListItems(); // ���� ������ ����

    
    for (const auto& ItemPair : itemList)
    {
        AC_Item* Item = ItemPair.Value; // TMap���� ������ ��������
        if (Item)
        {
            // UC_ItemBarWidget �ν��Ͻ� ����
            //UC_ItemBarWidget* NewItemBarWidget = CreateWidget<UC_ItemBarWidget>(GetWorld(), UC_ItemBarWidget::StaticClass());
            list->GetEntryWidgetClass();
            list->AddItem(Item);
            //UC_ItemBarWidget* NewItemBarWidget = Cast<UC_ItemBarWidget>(GetWidgetFromName(FName("WBP_ItemBarWidget")));
            //if (ItemBarInstance)
            //{
            //    // ������ ������ �ʱ�ȭ
            //    ItemBarInstance->InitBar(Item); // InitBar �޼��带 ���� ������ ������ ����
            //    list->AddItem(ItemBarInstance); // ����Ʈ�信 ������ �߰�
            //}
        }
    }
}
