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

    InitListView();


    // 아이템 리스트 위젯 초기화 및 데이터 추가
    if (MyItemListWidget)
    {
        TMap<FString, AC_Item*> MyItems; // 실제 아이템 리스트를 가져오는 로직 필요
        MyItems = OwnerCharacter->GetInvenComponent()->GetTestMyItems();
        MyItemListWidget->SetVisibility(ESlateVisibility::Visible);

        MyItemListWidget->AddTMapItem(MyItems); // 아이템 리스트 추가
        PopulateItemList(MyItemListWidget->ItemListView, MyItems);
        //MyItemListWidget->AddToViewport();
    }

    if (IsValid(AroundItemListWidget))
    {
        //TMap<FString, AC_Item*> AroundItems; // 실제 아이템 리스트를 가져오는 로직 필요
        //AroundItems = OwnerCharacter->GetInvenComponent()->GetTestAroundItems();

        TArray<AC_Item*> TestAroundItemList;
        TestAroundItemList = OwnerCharacter->GetInvenComponent()->GetNearItems();
        AroundItemListWidget->SetVisibility(ESlateVisibility::Visible);
        //AroundItemListWidget->AddTMapItem(TestAroundItemList);
        testAroundItemList(AroundItemListWidget->ItemListView, TestAroundItemList);
        AroundItemListWidget->AddToViewport();
       //AroundItemListWidget->AddToViewport();
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

void UC_InvenUiWidget::InitWidget()
{
    //if (!IsValid(MyItemListWidget))
    //    //MyItemListWidget = Cast<UC_MyItemListWidget>(GetWidgetFromName(FName("MyItemListWidget1")));
    //
    //if (!IsValid(AroundItemListWidget))
    //    //AroundItemListWidget = Cast<UC_MyItemListWidget>(GetWidgetFromName(FName("AroundItemListWidget1")));
    //
    //if (!IsValid(MyItemListView))
    //    MyItemListView = Cast<UListView>(GetWidgetFromName(FName("MyItemList")));
    //
    //if (!IsValid(AroundItemListView))
    //    AroundItemListView = Cast<UListView>(GetWidgetFromName(FName("AroundItemList")));
    //
    //if (!IsValid(MainGunSlot))
    //    MainGunSlot = Cast<UC_MainGunWidget>(GetWidgetFromName(FName("WB_MainGun")));
    //
    //if (!IsValid(SubGunSlot))
    //    SubGunSlot = Cast<UC_MainGunWidget>(GetWidgetFromName(FName("WB_SubGun")));
    //
    //if (!IsValid(MeleeSlot))
    //    MeleeSlot = Cast<UC_ThrowableWidget>(GetWidgetFromName(FName("WB_Melee")));
    //
    //if (!IsValid(ThrowableSlot))
    //    ThrowableSlot = Cast<UC_ThrowableWidget>(GetWidgetFromName(FName("WB_Throwble")));
    //
    //if (!IsValid(Background_Around))
    //    Background_Around = Cast<UUserWidget>(GetWidgetFromName(FName("WB_Background_Around")));
    //
    //if (!IsValid(Background_Inventory))
    //    Background_Around = Cast<UUserWidget>(GetWidgetFromName(FName("WB_Background_Inventory")));

    SetWidgetsOwner(OwnerCharacter);
    
    if (!IsValid(MainGunSlot)) return;

    MainGunSlot->SetWeaponBoxNum(1);
    //MainGunSlot->Init();

    if (!IsValid(SubGunSlot)) return;
    SubGunSlot->SetWeaponBoxNum(2);
    //SubGunSlot->Init();

    if (!IsValid(MeleeSlot)) return;
    MeleeSlot->SetWeaponBoxNum(4);
    //MeleeSlot->Init();

    if (!IsValid(ThrowableSlot)) return;
    ThrowableSlot->SetWeaponBoxNum(5);
    //ThrowableSlot->Init();



}

void UC_InvenUiWidget::SetItemListZorder(AC_BasicCharacter* Character)
{
    InitWidget();

    //이것 외의 방법은 미리 Z-order를 1로 올려두고 Visibility를 조정하는 방법이 있다.
    if (Character)
    {
        if (!IsValid(Background_Around)) return;
        Background_Around   ->AddToViewport(2);

        if (!IsValid(Background_Inventory)) return;
        Background_Inventory->AddToViewport(0);
        //Background_Around->SetVisibility(ESlateVisibility::Visible);
        //Background_Inventory->SetVisibility(ESlateVisibility::Hidden);
       

    }
    else
    {
        if (!IsValid(Background_Around)) return;
        Background_Around   ->AddToViewport(0);

        if (!IsValid(Background_Inventory)) return;
        Background_Inventory->AddToViewport(2);

        //Background_Around->SetVisibility(ESlateVisibility::Hidden);
        //Background_Inventory->SetVisibility(ESlateVisibility::Visible);
    }
}

void UC_InvenUiWidget::InitItemListZorder()
{
    if (!IsValid(Background_Around)) return;
    if (!IsValid(Background_Inventory)) return;

    Background_Around->AddToViewport(0);
    Background_Inventory->AddToViewport(0);
    //Background_Around->SetVisibility(ESlateVisibility::Hidden);
    //Background_Inventory->SetVisibility(ESlateVisibility::Hidden);
}



void UC_InvenUiWidget::SetWidgetsOwner(AC_BasicCharacter* Character)
{
    if (IsValid(MainGunSlot))
        MainGunSlot  ->SetOwnerCharacter(Character);
    if (IsValid(SubGunSlot))
        SubGunSlot   ->SetOwnerCharacter(Character);
    if (IsValid(MeleeSlot))
        MeleeSlot    ->SetOwnerCharacter(Character);
    if (IsValid(ThrowableSlot))
        ThrowableSlot->SetOwnerCharacter(Character);

}

void UC_InvenUiWidget::InitListView()
{
    //dnseInitWidget();

    //아이템 리스트 위젯 초기화 및 데이터 추가
    if (IsValid(OwnerCharacter) && IsValid(OwnerCharacter->GetInvenComponent()))
    {
        //MyItemListView->AddToViewport();
        //MyItemListView->SetVisibility(ESlateVisibility::Hidden);
        //MyItemListView->ClearListItems();
        TMap<FString, AC_Item*> MyItems; // 실제 아이템 리스트를 가져오는 로직 필요
        MyItems = OwnerCharacter->GetInvenComponent()->GetTestMyItems();

        //PopulateItemList(MyItemListView, MyItems);

        //MyItemListView->RequestRefresh();

        //MyItemListView->SetVisibility(ESlateVisibility::Visible);

    }
    else
        return;

    if (IsValid(AroundItemListView) && IsValid(OwnerCharacter->GetInvenComponent()))
    {
        //AroundItemListView->SetVisibility(ESlateVisibility::Hidden);

        //AroundItemListView->AddToViewport();
        //AroundItemListView->ClearListItems();
        TMap<FString, AC_Item*> AroundItems; // 실제 아이템 리스트를 가져오는 로직 필요
        AroundItems = OwnerCharacter->GetInvenComponent()->GetTestAroundItems();
        
        TArray<AC_Item*> TestAroundItemList;
        TestAroundItemList = OwnerCharacter->GetInvenComponent()->GetNearItems();

        //PopulateItemList(AroundItemListView, AroundItems);
        //testAroundItemList(AroundItemListView, TestAroundItemList);
        //AroundItemListView->RequestRefresh();

        //AroundItemListView->SetVisibility(ESlateVisibility::Visible);
    }
    else
        return;

    if (MyItemListWidget)
    {
        TMap<FString, AC_Item*> MyItems; // 실제 아이템 리스트를 가져오는 로직 필요
        MyItems = OwnerCharacter->GetInvenComponent()->GetTestMyItems();
        MyItemListWidget->SetVisibility(ESlateVisibility::Visible);

        MyItemListWidget->AddTMapItem(MyItems); // 아이템 리스트 추가
        PopulateItemList(MyItemListWidget->ItemListView, MyItems);
        //MyItemListWidget->AddToViewport();
    }

    if (IsValid(AroundItemListWidget))
    {
        //TMap<FString, AC_Item*> AroundItems; // 실제 아이템 리스트를 가져오는 로직 필요
        //AroundItems = OwnerCharacter->GetInvenComponent()->GetTestAroundItems();

        TArray<AC_Item*> TestAroundItemList;
        TestAroundItemList = OwnerCharacter->GetInvenComponent()->GetNearItems();
        AroundItemListWidget->SetVisibility(ESlateVisibility::Visible);
        //AroundItemListWidget->AddTMapItem(TestAroundItemList);
        //testAroundItemList(AroundItemListWidget->ItemListView, TestAroundItemList);
        AroundItemListWidget->InitAroundItemList(TestAroundItemList);
        //AroundItemListWidget->AddToViewport();
    }
}

void UC_InvenUiWidget::PopulateItemList(UListView* list, const TMap<FString, AC_Item*>& itemList)
{
    if (!IsValid(list))
    {
        UC_Util::Print("void List");
        return;
    }
    
    list->ClearListItems(); // 기존 아이템 삭제
    
    //if (itemList)

    if (!(itemList.Num() > 0)) return;
    for (const auto& ItemPair : itemList)
    {
         AC_Item* Item = ItemPair.Value; // TMap에서 아이템 가져오기
         if (IsValid(Item))
         {
             list->AddItem(Item);
         }
         else
         {
             return;
         }
         //ItemBar갱신.
         UC_ItemBarWidget* EntryWidget = Cast<UC_ItemBarWidget>(MyItemListView->GetEntryWidgetFromItem(Item));
         //UC_ItemBarWidget* EntryWidget = Cast<UC_ItemBarWidget>(MyItemListWidget->ItemListBar->GetEntryWidgetFromItem(Item));

         if (IsValid(EntryWidget))
             EntryWidget->InitBar(Item);
    }

    //list->RequestRefresh();
}

void UC_InvenUiWidget::testAroundItemList(UListView* list, const TArray<AC_Item*>& AroundItemList)
{
    if (!IsValid(list))
    {
        UC_Util::Print("void List");
        return;
    }

    list->ClearListItems(); // 기존 아이템 삭제

    if (!(AroundItemList.Num() > 0)) return;

    for (auto& AroundItem : AroundItemList)
    {
        AC_Item* Item = AroundItem;
        if (IsValid(Item))
        {
            list->AddItem(Item);
        }
        else
        {
            return;
        }
        //UC_ItemBarWidget* EntryWidget = Cast<UC_ItemBarWidget>(MyItemListView->GetEntryWidgetFromItem(Item));
        UC_ItemBarWidget* EntryWidget = Cast<UC_ItemBarWidget>(AroundItemListView->GetEntryWidgetFromItem(Item));

        if (EntryWidget)
            EntryWidget->InitBar(Item);
    }
}

