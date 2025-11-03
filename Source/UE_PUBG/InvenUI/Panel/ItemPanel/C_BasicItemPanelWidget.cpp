// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/Panel/ItemPanel/C_BasicItemPanelWidget.h"
#include "InvenUI/ItemBar/C_BasicItemBarWidget.h"
#include "Character/C_Player.h"
#include "Character/Component/C_InvenComponent.h"
#include "Item/C_Item.h"
#include "Item/ConsumableItem/C_ConsumableItem.h"
#include "Item/C_ItemDataObject.h"
#include "Singleton/C_GameInstance.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"


void UC_BasicItemPanelWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // DataObject 풀 초기화
    for (int32 i = 0; i < MaxPoolSize; ++i)
    {
        UC_ItemDataObject* Obj = NewObject<UC_ItemDataObject>(this);
        DataObjectPool.Add(Obj);
    }

    if (ItemListView)
    {
        ItemListView->SetVisibility(ESlateVisibility::Visible);
        ItemListView->SetSelectionMode(ESelectionMode::None); // 포커스 방지
        ItemListView->ClearListItems();

    }
}
//현재 이 업데이트 구조는 쓰레기임.
//나중에 바꾼다면 델리게이트를 적극활용하든 해서 업데이트가 필요한 ItemBarWidget들만 업데이트하도록 해야 한다.
//지금처럼 모조리 업데이트하는 구조는 쓰레기임.
void UC_BasicItemPanelWidget::UpdateInventoryItemList(TMap<FName, TArray<AC_Item*>> MyItemMap)
{
    if (!IsValid(ItemListView)) return;

    TArray<UC_ItemDataObject*> ItemsToAdd;

    for (const auto& MyItemPair : MyItemMap)
    {
        for (AC_Item* Item : MyItemPair.Value)
        {
            if (!IsValid(Item)) continue;

            UC_ItemDataObject* DataObj = GetDataObject();
            DataObj->ItemDataRef = Item->GetItemDatas();
            DataObj->SetItemCode(Item->GetItemCode());
            DataObj->SetItemCurStack(Item->GetItemCurStack());
            DataObj->SetItemPlace(Item->GetItemPlace());
            DataObj->SetOwnerCharacter(Item->GetOwnerCharacter());
            DataObj->ItemRef = Item;
            DataObj->UseProgress = Item->GetUseProgress();

            ItemsToAdd.Add(DataObj);
        }
    }

    ItemsToAdd.Sort([](const UC_ItemDataObject& A, const UC_ItemDataObject& B)
        {
            return A.ItemDataRef->ItemType < B.ItemDataRef->ItemType;
        });

    ItemListView->SetListItems(ItemsToAdd);
    ItemListView->RequestRefresh();
    ItemListView->ClearSelection();
}

void UC_BasicItemPanelWidget::UpdateAroundItemList(const TArray<AC_Item*>& AroundItemList)
{
    if (!IsValid(ItemListView) || AroundItemList.Num() == 0) return;

    TArray<UC_ItemDataObject*> DataObjects;
    for (AC_Item* Item : AroundItemList)
    {
        if (!IsValid(Item)) continue;

        UC_ItemDataObject* DataObj = GetDataObject();
        DataObj->ItemDataRef = Item->GetItemDatas();
        DataObj->SetItemCode(Item->GetItemCode());
        DataObj->SetItemCurStack(Item->GetItemCurStack());
        DataObj->SetItemPlace(Item->GetItemPlace());
        DataObj->SetOwnerCharacter(Item->GetOwnerCharacter());
        DataObj->ItemRef = Item;
        DataObj->UseProgress = Item->GetUseProgress();

        DataObjects.Add(DataObj);
    }

    DataObjects.Sort([](const UC_ItemDataObject& A, const UC_ItemDataObject& B)
        {
            return A.ItemDataRef->ItemType < B.ItemDataRef->ItemType;
        });

    ItemListView->SetListItems(DataObjects);
    ItemListView->RequestRefresh();
    ItemListView->ClearSelection();

}

void UC_BasicItemPanelWidget::AddItemToInventoryItemList(AC_Item* InItem)
{
    if (!IsValid(InItem) || !IsValid(ItemListView)) return;

    UC_ItemDataObject* DataObj = GetDataObject();
    DataObj->ItemDataRef = InItem->GetItemDatas();
    DataObj->SetItemCode(InItem->GetItemCode());
    DataObj->SetItemCurStack(InItem->GetItemCurStack());
    DataObj->SetItemPlace(InItem->GetItemPlace());
    DataObj->SetOwnerCharacter(InItem->GetOwnerCharacter());
    DataObj->ItemRef = InItem;

    ItemListView->AddItem(DataObj);
    ActiveDataObjects.Add(DataObj);
}

void UC_BasicItemPanelWidget::AddItemToAroundItemList(AC_Item* InItem)
{
    AddItemToInventoryItemList(InItem);
}

void UC_BasicItemPanelWidget::AddItemToList(AC_Item* InItem)
{
	//if (!IsValid(InItem) || !IsValid(ItemListView)) return;
	//UC_ItemDataObject* DataObj = NewObject<UC_ItemDataObject>(this);
	//DataObj->ItemDataRef = InItem->GetItemDatas();
	//DataObj->SetItemCode(InItem->GetItemCode());
	//DataObj->SetItemCurStack(InItem->GetItemCurStack());
	//DataObj->SetItemPlace(InItem->GetItemPlace());
	//DataObj->SetOwnerCharacter(InItem->GetOwnerCharacter());
	//DataObj->ItemRef = InItem; // 원본 아이템 참조 설정
	//ItemListView->AddItem(DataObj);
    if (!IsValid(InItem) || !IsValid(ItemListView)) return;

    UC_ItemDataObject* DataObj = GetDataObject();

    DataObj->ItemDataRef = InItem->GetItemDatas();
    DataObj->SetItemCode(InItem->GetItemCode());
    DataObj->SetItemCurStack(InItem->GetItemCurStack());
    DataObj->SetItemPlace(InItem->GetItemPlace());
    DataObj->SetOwnerCharacter(InItem->GetOwnerCharacter());
    DataObj->ItemRef = InItem;

    ItemListView->AddItem(DataObj);

    ActiveDataObjects.Add(DataObj);

    if (!IsValid(DataObj->GetOwnerCharacter())) return;

    if (AC_ConsumableItem* ConsumableItem = Cast<AC_ConsumableItem>(InItem))
    {
        // ListView에서 해당 아이템에 대한 EntryWidget 가져오기 (이제 유효할 가능성이 높음)
        UC_BasicItemBarWidget* EntryWidget = Cast<UC_BasicItemBarWidget>(ItemListView->GetEntryWidgetFromItem(DataObj));
        if (IsValid(EntryWidget))
            ConsumableItem->SetLinkedItemBarWidget(EntryWidget);
        if (IsValid(ConsumableItem->GetLinkedItemBarWidget()))
        {
            EntryWidget = ConsumableItem->GetLinkedItemBarWidget();
        }
        EntryWidget->UpdateWidget(DataObj);
    }

}

void UC_BasicItemPanelWidget::RemoveItemInList(AC_Item* InItem)
{
    //ItemListView1->RemoveItem(InItem);
    TArray<UObject*> ItemList = ItemListView->GetListItems();

	for (UObject* Obj : ItemList)
	{
		UC_ItemDataObject* DataObj = Cast<UC_ItemDataObject>(Obj);
		if (IsValid(DataObj) && DataObj->ItemRef == InItem)
		{
			ItemListView->RemoveItem(DataObj);
            ActiveDataObjects.Remove(DataObj);

            ReturnDataObject(DataObj);
			break;
		}
	}
}

void UC_BasicItemPanelWidget::RemoveItemInList(UC_ItemDataObject* InDataObj)
{
    if (!IsValid(InDataObj) || !IsValid(ItemListView)) return;

    ItemListView->RemoveItem(InDataObj);
    ActiveDataObjects.Remove(InDataObj);
    ReturnDataObject(InDataObj);
}

void UC_BasicItemPanelWidget::PreInitializeItemBars()
{
    if (!ItemListView) return;

    ItemListView->ClearListItems();

    for (int32 i = 0; i < MaxPoolSize; ++i)
    {
        UC_ItemDataObject* DataObj = GetDataObject();
        ItemListView->AddItem(DataObj);
    }

    ItemListView->RequestRefresh();
    ItemListView->ClearListItems(); // Slate 구조만 미리 빌드
}

UC_ItemDataObject* UC_BasicItemPanelWidget::GetDataObject()
{
    for (UC_ItemDataObject* Obj : DataObjectPool)
    {
        if (!ActiveDataObjects.Contains(Obj))
        {
            ActiveDataObjects.Add(Obj);
            return Obj;
        }
    }
    // 풀 부족 시 새로 생성
    UC_ItemDataObject* NewObj = NewObject<UC_ItemDataObject>(this);
    ActiveDataObjects.Add(NewObj);
    return NewObj;
}

void UC_BasicItemPanelWidget::ReturnDataObject(UC_ItemDataObject* Obj)
{
    if (!Obj) return;
    ActiveDataObjects.Remove(Obj);
    // 필요 시 초기화
    Obj->ItemRef = nullptr;
}
