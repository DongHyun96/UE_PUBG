// Fill out your copyright notice in the Description page of Project Settings.

#include "InvenUserInterface/C_InvenUiWidget.h"
<<<<<<< Updated upstream
=======
#include "InvenUserInterface/C_ItemBarWidget.h"
#include "InvenUserInterface/C_MainGunWidget.h"
#include "InvenUserInterface/C_ThrowableWidget.h"
#include "InvenUserInterface/C_EquipSlot.h"

>>>>>>> Stashed changes
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/ListView.h"
#include "UE_PUBG/Character/C_BasicCharacter.h"
#include "C_MyItemListWidget.h"

#include "Components/CanvasPanel.h"

<<<<<<< Updated upstream
#include "InvenUserInterface/C_ItemBarWidget.h"
=======
>>>>>>> Stashed changes

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
    MainGunSlot->SetWeaponBoxNum(1);
    MainGunSlot->Init();

    SubGunSlot = Cast<UC_MainGunWidget>(GetWidgetFromName(FName("WB_SubGun")));
    SubGunSlot->SetWeaponBoxNum(2);
    SubGunSlot->Init();

    MeleeSlot = Cast<UC_ThrowableWidget>(GetWidgetFromName(FName("WB_Melee")));
    MeleeSlot->SetWeaponBoxNum(4);

    ThrowableSlot = Cast<UC_ThrowableWidget>(GetWidgetFromName(FName("WB_Throwble")));
    ThrowableSlot->SetWeaponBoxNum(5);

    SetWidgetsOwner(OwnerCharacter);

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
            list->AddItem(Item);
        }
    }
}
