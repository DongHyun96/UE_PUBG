// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/Panel/ItemPanel/C_BasicItemPanelWidget.h"
#include "InvenUI/ItemBar/C_BasicItemBarWidget.h"
#include "Character/C_Player.h"
#include "Character/Component/C_InvenComponent.h"
#include "Item/C_Item.h"
#include "Item/ConsumableItem/C_ConsumableItem.h"
#include "Item/C_ItemDataObject.h"
#include "Singleton/C_GameInstance.h"
#include "Utility/C_Util.h"


void UC_BasicItemPanelWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 풀 초기화
    for (int32 i = 0; i < MaxPoolSize; ++i)
    {
        UC_ItemDataObject* Obj = NewObject<UC_ItemDataObject>(GetWorld());
        DataObjectPool.Add(Obj);
    }

    UC_GameInstance* GI = Cast<UC_GameInstance>(GetGameInstance());
    const FItemData* DummyData = nullptr;
	FName DummyItemCode = "Item_Weapon_Kar98k_C";
    DummyData = GI->GetItemData(DummyItemCode);
    //for (int i = 0; i < MaxPoolSize; ++i)
    //{
    //    UC_BasicItemBarWidget* TempWidget = CreateWidget<UC_BasicItemBarWidget>(GetWorld(), ItemBarWidgetClass);
    //
    //    // 임시 데이터 객체 생성
    //    UC_ItemDataObject* TempData = NewObject<UC_ItemDataObject>(TempWidget);
    //    TempData->ItemDataRef = DummyData; // 혹은 더미 FItemData
    //    TempWidget->NativeOnListItemObjectSet(TempData);
    //
    //    ItemListView->AddItem(TempData);
    //    ItemListView->RemoveItem(TempData);
    //}


    ItemListView->SetVisibility(ESlateVisibility::Hidden);
    ItemListView->SetVisibility(ESlateVisibility::Visible);
}

void UC_BasicItemPanelWidget::UpdateInventoryItemList(TMap<FName, TArray<AC_Item*>> MyItemMap)
{
    if (!IsValid(ItemListView)) return;

    TArray<UC_ItemDataObject*> ItemsToAdd;

    // AC_Item → UC_ItemDataObject 래핑
    for (const auto& MyItemPair : MyItemMap)
    {
        for (AC_Item* Item : MyItemPair.Value)
        {
            if (!IsValid(Item)) continue;

            UC_ItemDataObject* DataObj = NewObject<UC_ItemDataObject>(this);
            DataObj->ItemDataRef = Item->GetItemDatas();
            DataObj->SetItemCode(Item->GetItemCode());
            DataObj->SetItemCurStack(Item->GetItemCurStack());
            DataObj->SetItemPlace(Item->GetItemPlace());
            DataObj->SetOwnerCharacter(Item->GetOwnerCharacter());
			DataObj->ItemRef = Item; // 원본 아이템 참조 설정

            ItemsToAdd.Add(DataObj);
        }
    }

    // ItemType 기준 정렬
    ItemsToAdd.Sort([](UC_ItemDataObject& A, UC_ItemDataObject& B)
        {
            return A.ItemDataRef->ItemType < B.ItemDataRef->ItemType;
        });

    // ListView 갱신
    ItemListView->SetListItems(ItemsToAdd);
}

void UC_BasicItemPanelWidget::UpdateAroundItemList(const TArray<AC_Item*>& AroundItemList)
{
    ItemListView->ClearListItems();

    if (!IsValid(ItemListView) || AroundItemList.Num() == 0) return;


    TArray<UC_ItemDataObject*> DataObjects;
    for (AC_Item* Item : AroundItemList)
    {
        if (!IsValid(Item)) continue;

        UC_ItemDataObject* DataObj = NewObject<UC_ItemDataObject>(this);
        DataObj->ItemDataRef = Item->GetItemDatas();
        DataObj->SetItemCode(Item->GetItemCode());
        DataObj->SetItemCurStack(Item->GetItemCurStack());
        DataObj->SetItemPlace(Item->GetItemPlace());
        DataObj->SetOwnerCharacter(Item->GetOwnerCharacter());
        DataObj->ItemRef = Item; // 원본 아이템 참조 설정

        DataObjects.Add(DataObj);
    }

    // 정렬 가능
    DataObjects.Sort([](UC_ItemDataObject& A, UC_ItemDataObject& B)
        {
            return A.ItemDataRef->ItemType < B.ItemDataRef->ItemType;
        });

    ItemListView->SetListItems(DataObjects);

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
    //if (!IsValid(InDataObj) || !IsValid(ItemListView)) return;
    //
    //ItemListView->RemoveItem(InDataObj);

    if (!IsValid(InDataObj) || !IsValid(ItemListView)) return;

    ItemListView->RemoveItem(InDataObj);
    ActiveDataObjects.Remove(InDataObj);

    ReturnDataObject(InDataObj);
}

UC_ItemDataObject* UC_BasicItemPanelWidget::GetDataObject()
{
    if (DataObjectPool.Num() > 0)
    {
        return DataObjectPool.Pop();
    }

    // 풀 부족 시 새로 생성
    return NewObject<UC_ItemDataObject>(this);
}

void UC_BasicItemPanelWidget::ReturnDataObject(UC_ItemDataObject* Obj)
{
    if (!Obj) return;

    if (ItemListView->GetListItems().Contains(Obj))
    {
        ItemListView->RemoveItem(Obj);
    }

    Obj->ItemDataRef = nullptr;
    Obj->ItemRef = nullptr;

    DataObjectPool.Add(Obj);
}
