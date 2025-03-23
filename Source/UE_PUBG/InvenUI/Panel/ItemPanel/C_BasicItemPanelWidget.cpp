// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/Panel/ItemPanel/C_BasicItemPanelWidget.h"
#include "InvenUI/ItemBar/C_BasicItemBarWidget.h"
#include "Item/C_Item.h"
#include "Item/ConsumableItem/C_ConsumableItem.h"
#include "Utility/C_Util.h"

void UC_BasicItemPanelWidget::UpdateInventoryItemList(TMap<FName, TArray<AC_Item*>> MyItemMap)
{
    //if (!IsValid(ItemListView1)) return;

    //// 아이템 리스트를 먼저 비우고, 아이템 리스트내의 원소가 0이면 종료
    //ItemListView1->ClearListItems();
    //if (MyItemMap.Num() == 0) return;

    //// 한 번에 추가할 아이템들을 저장할 TArray 준비
    //TArray<UC_ItemBox*> ItemsToAdd;
    ////TArray<UC_BasicItemBarWidget*> EntryWidgetsToUpdate;

    //// 아이템 맵에서 아이템을 하나씩 처리
    //for (const auto& MyItem : MyItemMap)
    //{
    //    AC_Item* Item = MyItem.Value;

    //    // 아이템이 유효하지 않으면 건너뛰기
    //    if (!IsValid(Item)) continue;

    //    int32 MaxStack = Item->GetItemDatas()->ItemMaxStack; // 아이템의 최대 스택 크기 가져오기
    //    int32 TotalCount = Item->GetItemCurStack(); // 현재 아이템 개수
    //    
    //    while (TotalCount > 0)
    //    {
    //        int32 StackToAdd = FMath::Min(TotalCount, MaxStack);
    //        UC_ItemBox* NewItemBox = NewObject<UC_ItemBox>();
    //        //NewItemBox->GetItemRef() = Item;
    //        //NewItemBox->StackCount = StackToAdd;

    //        NewItemBox->Init(Item, StackToAdd);
    //    
    //        ItemsToAdd.Add(NewItemBox);

    //        ItemListView1->AddItem(NewItemBox);
    //    
    //        UC_BasicItemBarWidget* EntryWidget = Cast<UC_BasicItemBarWidget>(ItemListView1->GetEntryWidgetFromItem(NewItemBox));

    //        if (IsValid(EntryWidget))
    //        {
    //            // ConsumableItem에 대한 처리는 미리 해두기
    //            if (AC_ConsumableItem* ConsumableItem = Cast<AC_ConsumableItem>(NewItemBox->GetItemRef()))
    //            {
    //                if (IsValid(ConsumableItem->GetLinkedItemBarWidget()))
    //                {
    //                    EntryWidget = ConsumableItem->GetLinkedItemBarWidget();
    //                }
    //                else
    //                {
    //                    ConsumableItem->SetLinkedItemBarWidget(EntryWidget);
    //                }
    //            }
    //            EntryWidget->UpdateWidget(NewItemBox);
    //            // EntryWidget을 업데이트할 목록에 추가
    //            //EntryWidgetsToUpdate.Add(EntryWidget);
    //        }

    //        TotalCount -= StackToAdd;
    //    }
    //    //아이템을 추가할 리스트에 저장
    //    //ItemsToAdd.Add(Item);

    //    // 아이템 바 위젯 갱신 준비
    //   
    //}

    ////TODO : 아이템 우선순위 만들기, DataTable의 순서를 가져오는 방법은?
    ////ItemsToAdd.Sort([](const AC_Item& A, const AC_Item& B) {
    ////    return A.GetItemType() < B.GetItemType();
    ////    });

    //// 한 번에 아이템을 추가
    //if (ItemsToAdd.Num() > 0)
    //{
    //    ItemListView1->SetListItems(ItemsToAdd);
    //}
    //ItemListView1->RegenerateAllEntries();

    if (!IsValid(ItemListView1)) return;

    // 아이템 리스트 초기화
    ItemListView1->ClearListItems();
    if (MyItemMap.Num() == 0) return;

    // 추가할 아이템 목록
    TArray<AC_Item*> ItemsToAdd;

    // MyItemMap을 순회하면서 모든 아이템을 리스트에 추가
    for (const auto& MyItem : MyItemMap)
    {
        for (AC_Item* Item : MyItem.Value) // 배열 안의 모든 아이템 처리
        {
            if (IsValid(Item))
            {
                ItemsToAdd.Emplace(Item);
            }
        }
    }

    //아이템 타입(EItemType) 기준 정렬
    ItemsToAdd.Sort([](const AC_Item& A, const AC_Item& B) {
        return A.GetItemDatas()->ItemType < B.GetItemDatas()->ItemType;
        });

    //ListView에 아이템 추가
    if (ItemsToAdd.Num() > 0)
    {
        ItemListView1->SetListItems(ItemsToAdd); //여기서 먼저 추가

        //ConsumableItem의 LinkedItemBarWidget 설정
        for (AC_Item* Item : ItemsToAdd)
        {
            if (AC_ConsumableItem* ConsumableItem = Cast<AC_ConsumableItem>(Item))
            {
                // ListView에서 해당 아이템에 대한 EntryWidget 가져오기 (이제 유효할 가능성이 높음)
                UC_BasicItemBarWidget* EntryWidget = Cast<UC_BasicItemBarWidget>(ItemListView1->GetEntryWidgetFromItem(Item));

                if (IsValid(ConsumableItem->GetLinkedItemBarWidget()))
                {
                    EntryWidget = ConsumableItem->GetLinkedItemBarWidget();
                }
                else
                {
                    ConsumableItem->SetLinkedItemBarWidget(EntryWidget);
                }
                //EntryWidget->UpdateWidget(Item);
            }
            UC_Util::Print(Item->GetItemCurStack());
        }
    }
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

   //TArray<UC_ItemBox*> ItemsToAdd;
   //for (auto& AroundItem : SortedItems)
   //{
   //    UC_ItemBox* NewItemBox = NewObject<UC_ItemBox>();
   // 
   //    NewItemBox->Init(AroundItem, AroundItem->GetItemCurStack());
   //
   //    ItemsToAdd.Add(NewItemBox);
   //}




    // 한 번에 아이템을 추가
    if (SortedItems.Num() > 0)
    {
        ItemListView1->SetListItems(SortedItems);
    }

    //for (auto& AroundItem : SortedItems)
    //{
    //    AC_Item* Item = AroundItem;
    //    if (IsValid(Item))
    //    {
    //        ItemListView1->AddItem(Item);
    //    }
    //    else
    //    {
    //        return;
    //    }
    //    
    //    UC_BasicItemBarWidget* EntryWidget = Cast<UC_BasicItemBarWidget>(ItemListView1->GetEntryWidgetFromItem(Item));
    //
    //    if (EntryWidget)
    //        EntryWidget->UpdateWidget(Item);
    //}
}

void UC_BasicItemPanelWidget::RemoveItemInList(AC_Item* InItem)
{
    ItemListView1->RemoveItem(InItem);
}


