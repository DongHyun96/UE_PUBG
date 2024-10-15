// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUserInterface/C_ItemBarWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Item/C_Item.h"
#include "Utility/C_Util.h"

//void UC_ItemBarWidget::OnListItemObjectSet(UObject* ListItemObject)
//{
//    if (AC_Item* Item = Cast<AC_Item>(ListItemObject))
//    {
//        // 캐시된 아이템 설정
//        CachedItem = Item;
//
//        // 아이템 이름 설정
//        if (ItemNameText)
//        {
//            FText ItemName = FText::FromString(Item->GetItemDatas().ItemName); // ItemName은 AC_Item에 존재하는 변수라고 가정
//            ItemNameText->SetText(ItemName);
//        }
//
//        // 아이템 이미지 설정
//        if (ItemImage && Item->GetItemDatas().ItemIcon) // ItemTexture는 AC_Item에 정의된 텍스처 변수라고 가정
//        {
//            UTexture2D* Texture = Item->GetItemDatas().ItemIcon;
//            ItemImage->SetBrushFromTexture(Texture);
//        }
//    }
//}

void UC_ItemBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 바인딩된 위젯들이 null인지 확인
	if (!ItemImage)
	{
		UC_Util::Print("ItemImage is not bound correctly!",FColor::Red,10.f); 
	}

	if (!ItemName)
	{
		UC_Util::Print("ItemImage is not bound correctly!", FColor::Red, 10.f);
	}
}

void UC_ItemBarWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	// ListItemObject를 UC_Item 클래스로 캐스팅하여 아이템 데이터 사용
	AC_Item* Item = Cast<AC_Item>(ListItemObject);
	if (Item)
	{
		UC_Util::Print("true");
		CachedItem = Item;

		ItemType = Item->GetItemDatas().ItemType;

		// 아이템 이름을 텍스트 블록에 설정
		ItemName->SetText(FText::FromString(Item->GetItemDatas().ItemName));
		//ItemNameText->SetText(FText::FromString(Item->GetName()));
		
		ItemImage->SetBrushFromTexture(Item->GetItemDatas().ItemIcon);

		ItemStackBlock->SetText(FText::AsNumber(Item->GetItemDatas().ItemStack));

		ItemType = Item->GetItemDatas().ItemType;
	}
	else
	{
		UC_Util::Print("false");

	}
}

void UC_ItemBarWidget::InitBar(AC_Item* item)
{
	//if (CachedItem)
	//{

	//	ItemType = CachedItem->GetItemDatas().ItemType;

	//	ItemName->SetText(FText::FromString(CachedItem->GetItemDatas().ItemName));

	//	ItemImage->SetBrushFromTexture(CachedItem->GetItemDatas().ItemIcon);

	//	ItemStackBlock->SetText(FText::AsNumber(CachedItem->GetItemDatas().ItemStack));

	//	ItemType = CachedItem->GetItemDatas().ItemType;

	//	SetVisibility(ESlateVisibility::Visible);
	//}
	//else
	//{
	//	SetVisibility(ESlateVisibility::Hidden);
	//}
	if (item)
	{

		ItemType = item->GetItemDatas().ItemType;

		ItemName->SetText(FText::FromString(item->GetItemDatas().ItemName));

		ItemImage->SetBrushFromTexture(item->GetItemDatas().ItemIcon);

		ItemStackBlock->SetText(FText::AsNumber(item->GetItemDatas().ItemStack));

		ItemType = item->GetItemDatas().ItemType;

		SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		//SetVisibility(ESlateVisibility::Hidden);
		UC_Util::Print("Visibility::Hidden");
	}
}

AC_Item* UC_ItemBarWidget::GetItem(AC_Item* nearItem)
{
	return nullptr;
}

AC_Item* UC_ItemBarWidget::DropItem(AC_Item* myItem)
{
	return nullptr;
}

