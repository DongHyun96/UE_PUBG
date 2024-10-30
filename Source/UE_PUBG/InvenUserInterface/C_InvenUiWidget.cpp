// Fill out your copyright notice in the Description page of Project Settings.

#include "InvenUserInterface/C_InvenUiWidget.h"

#include "InvenUserInterface/C_ItemBarWidget.h"
#include "InvenUserInterface/C_MainGunWidget.h"
#include "InvenUserInterface/C_ThrowableWidget.h"
#include "InvenUserInterface/C_EquipSlot.h"


#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/ListView.h"
#include "UE_PUBG/Character/C_BasicCharacter.h"
#include "C_MyItemListWidget.h"

#include "Components/CanvasPanel.h"

#include "TimerManager.h"

#include "InvenUserInterface/C_ItemBarWidget.h"

#include "Utility/C_Util.h"

void UC_InvenUiWidget::NativeConstruct()
{
    Super::NativeConstruct();

    //InvenCanvas = Cast<UCanvasPanel>(GetWidgetFromName(FName("InvenCanvas1")));
    
    if (InvenCanvas)
    {
        
    }

    InitWidget();


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

void UC_InvenUiWidget::InitWidget()
{
    MyItemListWidget = Cast<UC_MyItemListWidget>(GetWidgetFromName(FName("MyItemListWidget1")));

    AroundItemListWidget = Cast<UC_MyItemListWidget>(GetWidgetFromName(FName("AroundItemListWidget1")));

    MyItemListView = Cast<UListView>(GetWidgetFromName(FName("MyItemList")));

    AroundItemListView = Cast<UListView>(GetWidgetFromName(FName("AroundItemList")));

    MainGunSlot = Cast<UC_MainGunWidget>(GetWidgetFromName(FName("WB_MainGun")));

    SubGunSlot = Cast<UC_MainGunWidget>(GetWidgetFromName(FName("WB_SubGun")));

    MeleeSlot = Cast<UC_ThrowableWidget>(GetWidgetFromName(FName("WB_Melee")));

    ThrowableSlot = Cast<UC_ThrowableWidget>(GetWidgetFromName(FName("WB_Throwble")));

    SetWidgetsOwner(OwnerCharacter);
    
    MainGunSlot->SetWeaponBoxNum(1);
    MainGunSlot->Init();
    
    SubGunSlot->SetWeaponBoxNum(2);
    SubGunSlot->Init();
    
    MeleeSlot->SetWeaponBoxNum(4);
    MeleeSlot->Init();
    
    ThrowableSlot->SetWeaponBoxNum(5);
    ThrowableSlot->Init();

    InitListView();


}

void UC_InvenUiWidget::SetWidgetsOwner(AC_BasicCharacter* Character)
{
    MainGunSlot  ->SetOwnerCharacter(Character);
    SubGunSlot   ->SetOwnerCharacter(Character);
    MeleeSlot    ->SetOwnerCharacter(Character);
    ThrowableSlot->SetOwnerCharacter(Character);
}

void UC_InvenUiWidget::InitListView()
{
    //     ������ ����Ʈ ���� �ʱ�ȭ �� ������ �߰�
    if (MyItemListView)
    { 
        //MyItemListView->AddToViewport();
        //MyItemListView->SetVisibility(ESlateVisibility::Hidden);
        //MyItemListView->ClearListItems();
        TMap<FString, AC_Item*> MyItems; // ���� ������ ����Ʈ�� �������� ���� �ʿ�
        MyItems = OwnerCharacter->GetInvenComponent()->GetTestMyItems();

        PopulateItemList(MyItemListView, MyItems);

        MyItemListView->RequestRefresh();

        MyItemListView->SetVisibility(ESlateVisibility::Visible);
    }

    if (AroundItemListView)
    {
        //AroundItemListView->SetVisibility(ESlateVisibility::Hidden);

        //AroundItemListView->AddToViewport();
        //AroundItemListView->ClearListItems();
        TMap<FString, AC_Item*> AroundItems; // ���� ������ ����Ʈ�� �������� ���� �ʿ�
        AroundItems = OwnerCharacter->GetInvenComponent()->GetTestAroundItems();

        PopulateItemList(AroundItemListView, AroundItems);

        //AroundItemListView->RequestRefresh();

        AroundItemListView->SetVisibility(ESlateVisibility::Visible);
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
            list->AddItem(Item);
        }
        //ItemBar����.
        UC_ItemBarWidget* EntryWidget = Cast<UC_ItemBarWidget>(MyItemListView->GetEntryWidgetFromItem(Item));
        if (EntryWidget)
            EntryWidget->InitBar(Item);
    }

    list->RequestRefresh();

}

void UC_InvenUiWidget::test(UListView* list, const TMap<FString, AC_Item*>& itemList)
{
    for (const auto& ItemPair : itemList)
    {
        AC_Item* Item = ItemPair.Value; // TMap���� ������ ��������
        if (Item)
        {
            list->AddItem(Item);
        }
        //ItemBar����.
        UC_ItemBarWidget* EntryWidget = Cast<UC_ItemBarWidget>(MyItemListView->GetEntryWidgetFromItem(Item));
        if (EntryWidget)
            EntryWidget->InitBar(Item);
    }

}
