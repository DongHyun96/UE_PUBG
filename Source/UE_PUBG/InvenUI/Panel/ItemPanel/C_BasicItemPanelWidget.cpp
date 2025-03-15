// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/Panel/ItemPanel/C_BasicItemPanelWidget.h"
#include "InvenUI/ItemBar/C_BasicItemBarWidget.h"
#include "Item/C_Item.h"
#include "Item/ConsumableItem/C_ConsumableItem.h"

//void UC_BasicItemPanelWidget::NativeConstruct()
//{
//    Super::NativeConstruct();
//
//    if (IsValid(ItemListView1))
//    {
//        ItemListView1->OnEntryInitialized().AddUObject(this, &UC_BasicItemPanelWidget::OnEntryInitialized);
//    }
//}
//
//void UC_BasicItemPanelWidget::OnEntryInitialized(UUserWidget& EntryWidget, AC_Item* ListItem)
//{
//    if (UC_BasicItemBarWidget* ItemBar = Cast<UC_BasicItemBarWidget>(&EntryWidget))
//    {
//        if (AC_Item* Item = Cast<AC_Item>(&ListItem))
//        {
//            ItemBar->UpdateWidget(Item);
//        }
//    }
//}

void UC_BasicItemPanelWidget::AddTMapItem(TMap<FString, TArray<AC_Item*>> MyItemMap)
{
    if (!IsValid(ItemListView1)) return;
    
    ItemListView1->ClearListItems(); // 기존 아이템 삭제

    if (MyItemMap.Num() == 0) return;

    for (const auto& ItemPairTArray : MyItemMap)
    {
        TArray<AC_Item*> ItemTArray = ItemPairTArray.Value; // TMap에서 아이템 가져오기

        for (const auto& Item : ItemTArray)
        {
            if (IsValid(Item))
            {
                ItemListView1->AddItem(Item);
            }
            else
            {
                continue;
            }

            //ItemBar갱신.
            UC_BasicItemBarWidget* EntryWidget = Cast<UC_BasicItemBarWidget>(ItemListView1->GetEntryWidgetFromItem(Item));
            //UC_ItemBarWidget* EntryWidget = Cast<UC_ItemBarWidget>(MyItemListWidget->ItemListBar->GetEntryWidgetFromItem(Item));
            
            if (IsValid(EntryWidget))
            {
                //if (AC_ConsumableItem* ConsumableItem = Cast<AC_ConsumableItem>(Item))
                //{
                //    if (IsValid(ConsumableItem->GetTestLinkedItemBarWidget()))
                //    {
                //        EntryWidget = ConsumableItem->GetTestLinkedItemBarWidget();
                //    }
                //    else
                //    {
                //        ConsumableItem->SetLinkedItemBarWidget(EntryWidget);
                //    }
                //}
                EntryWidget->UpdateWidget(Item);
            }
        }
    }
}

void UC_BasicItemPanelWidget::UpdateInventoryItemList(TMap<FName, AC_Item*> MyItemMap)
{
    if (!IsValid(ItemListView1)) return;

    // 아이템 리스트를 먼저 비우고, 아이템 리스트내의 원소가 0이면 종료
    ItemListView1->ClearListItems();
    if (MyItemMap.Num() == 0) return;

    // 한 번에 추가할 아이템들을 저장할 TArray 준비
    TArray<FInventoryItemBox*> ItemsToAdd;
    //TArray<UC_BasicItemBarWidget*> EntryWidgetsToUpdate;

    // 아이템 맵에서 아이템을 하나씩 처리
    for (const auto& MyItem : MyItemMap)
    {
        AC_Item* Item = MyItem.Value;

        // 아이템이 유효하지 않으면 건너뛰기
        if (!IsValid(Item)) continue;

        int32 MaxStack = Item->GetItemDatas()->ItemMaxStack; // 아이템의 최대 스택 크기 가져오기
        int32 TotalCount = Item->GetItemCurStack(); // 현재 아이템 개수

        while (TotalCount > 0)
        {
            int32 StackToAdd = FMath::Min(TotalCount, MaxStack);
            FInventoryItemBox* NewItemBox = NewObject<FInventoryItemBox>();
            NewItemBox->ItemRef = Item;
            NewItemBox->StackCount = StackToAdd;

            ItemsToAdd.Add(NewItemBox);

            TotalCount -= StackToAdd;
        }
        // 아이템을 추가할 리스트에 저장
        //ItemsToAdd.Add(Item);

        // 아이템 바 위젯 갱신 준비
        UC_BasicItemBarWidget* EntryWidget = Cast<UC_BasicItemBarWidget>(ItemListView1->GetEntryWidgetFromItem(Item));

        if (IsValid(EntryWidget))
        {
            // ConsumableItem에 대한 처리는 미리 해두기
            if (AC_ConsumableItem* ConsumableItem = Cast<AC_ConsumableItem>(Item))
            {
                if (IsValid(ConsumableItem->GetLinkedItemBarWidget()))
                {
                    EntryWidget = ConsumableItem->GetLinkedItemBarWidget();
                }
                else
                {
                    ConsumableItem->SetLinkedItemBarWidget(EntryWidget);
                }
            }
            EntryWidget->UpdateWidget(Item);
            // EntryWidget을 업데이트할 목록에 추가
            //EntryWidgetsToUpdate.Add(EntryWidget);
        }
    }

    //TODO : 아이템 우선순위 만들기, DataTable의 순서를 가져오는 방법은?
    ItemsToAdd.Sort([](const AC_Item& A, const AC_Item& B) {
        return A.GetItemType() < B.GetItemType();
        });

    // 한 번에 아이템을 추가
    if (ItemsToAdd.Num() > 0)
    {
        ItemListView1->SetListItems(ItemsToAdd);
    }
    //ItemListView1->RegenerateAllEntries();
}

void UC_BasicItemPanelWidget::UpdateAroundItemList(const TArray<AC_Item*>& AroundItemList)
{
    if (!IsValid(ItemListView1)) return;

    ItemListView1->ClearListItems(); // 기존 아이템 삭제
    //ItemListView1->SetListItems
    if (!(AroundItemList.Num() > 0)) return;

    TArray<AC_Item*> SortedItems = AroundItemList;

    //오름차순 정렬
    SortedItems.Sort([](const AC_Item& A, const AC_Item& B) {
        return A.GetItemName() < B.GetItemName(); 
        });

    for (auto& AroundItem : SortedItems)
    {
        AC_Item* Item = AroundItem;
        if (IsValid(Item))
        {
            ItemListView1->AddItem(Item);
        }
        else
        {
            return;
        }
        
        UC_BasicItemBarWidget* EntryWidget = Cast<UC_BasicItemBarWidget>(ItemListView1->GetEntryWidgetFromItem(Item));

        if (EntryWidget)
            EntryWidget->UpdateWidget(Item);
    }
}

void UC_BasicItemPanelWidget::RemoveItemInList(AC_Item* InItem)
{
    ItemListView1->RemoveItem(InItem);
}


