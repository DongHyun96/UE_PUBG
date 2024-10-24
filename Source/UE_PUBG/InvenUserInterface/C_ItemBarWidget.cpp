// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUserInterface/C_ItemBarWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Item/C_Item.h"
#include "InvenUserInterface/C_InvenUiWidget.h"
#include "Utility/C_Util.h"

void UC_ItemBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 위젯을 WidgetTree에서 가져오기
	//ItemImage = Cast<UImage>(GetWidgetFromName(FName("ItemImage1")));
	//ItemName = Cast<UTextBlock>(GetWidgetFromName(FName("ItemName1")));
	//ItemStackBlock = Cast<UTextBlock>(GetWidgetFromName(FName("ItemStackBlock1")));

	//ItemImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), FName("ItemImage1"));
	//ItemName = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName("ItemName1"));
	//ItemStackBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName("ItemStackBlock1"));

	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<AC_BasicCharacter>(GetOwningPlayerPawn());
	}



	// 바인딩된 위젯들이 null인지 확인
	if (!ItemImage1)
	{
		UC_Util::Print("ItemImage is not bound correctly!", FColor::Red, 10.f);
	}
	if (!ItemName1)
	{
		UC_Util::Print("ItemName is not bound correctly!", FColor::Red, 10.f);
	}
	if (!ItemStackBlock1)
	{
		UC_Util::Print("ItemStackBlock is not bound correctly!", FColor::Red, 10.f);
	}
}

void UC_ItemBarWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	// ListItemObject를 UC_Item 클래스로 캐스팅하여 아이템 데이터 사용
	CachedItem = Cast<AC_Item>(ListItemObject);

	if (!CachedItem) return;

	InitBar(CachedItem);
}

FReply UC_ItemBarWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 우클릭인지 체크
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		if (CachedItem)
		{   // 우클릭 이벤트 실행
			CachedItem->Interaction(OwnerCharacter);

			if (UC_InvenUiWidget* InvenUiWidget = GetTypedOuter<UC_InvenUiWidget>())
			{
				if (
						CachedItem->GetItemDatas().ItemType == EItemTypes::CONSUMPTIONITEM
						|| CachedItem->GetItemDatas().ItemType == EItemTypes::THROWABLE
						|| CachedItem->GetItemDatas().ItemType == EItemTypes::MELEEWEAPON
						|| CachedItem->GetItemDatas().ItemType == EItemTypes::ATTACHMENT
				   )
				{
					InvenUiWidget->InitListView();
				}
				
				InvenUiWidget->InitWidget();
			}
			//NativeOnListItemObjectSet에서의 호출과 중복으로 일단 주석처리, 다만 이벤트시에 초기화가 필요하면 사용해야 할 수 있음.
			//InitBar();
			return FReply::Handled();
		}
	}
	else
	{
		UC_Util::Print("No cached item to interact with!", FColor::Red, 5.0f);
	}
	// 다른 버튼 클릭 처리
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UC_ItemBarWidget::InitBar(AC_Item* item)
{


	//ItemImage = Cast<UImage>(GetWidgetFromName(FName("ItemImage1")));
	//ItemName = Cast<UTextBlock>(GetWidgetFromName(FName("ItemName1")));
	//ItemStackBlock = Cast<UTextBlock>(GetWidgetFromName(FName("ItemStackBlock1")));

	if (item)
	{
		CachedItem = item;

		ItemImage1->SetBrushFromTexture(item->GetItemDatas().ItemIcon);

		ItemType = item->GetItemDatas().ItemType;

		ItemName1->SetText(FText::FromString(item->GetItemDatas().ItemName));

		ItemStackBlock1->SetText(FText::AsNumber(item->GetItemDatas().ItemStack));
		//AddToViewport();
		SetVisibility(ESlateVisibility::Visible);

	}
	else
	{
		UC_Util::Print("NO CachedItem!!", FColor::Red, 5.0f);
		SetVisibility(ESlateVisibility::Hidden);
		//RemoveFromViewport();
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

