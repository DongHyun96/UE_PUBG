// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/ItemBar/C_BasicItemBarWidget.h"
#include "InvenUserInterface/C_InvenUiWidget.h"
#include "InvenUserInterface/C_DragDropOperation.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Progressbar.h"

//#include "Blueprint/WidgetBlueprintLibrary.h"

void UC_BasicItemBarWidget::NativeConstruct()
{

}

FReply UC_BasicItemBarWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		//우클릭 이벤트 실행
		//TODO : 구현
		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UC_BasicItemBarWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		//드래그 시작
		FEventReply RePlyResult =
			UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
		//TODO : 구현


		return RePlyResult.NativeReply;
	}
	return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);

}

void UC_BasicItemBarWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	UC_DragDropOperation* DragOperation = NewObject<UC_DragDropOperation>();
}

void UC_BasicItemBarWidget::UpdateWidget(AC_Item* MyItem)
{
	if (MyItem)
	{
		CachedItem = MyItem;

		ItemImage->SetBrushFromTexture(MyItem->GetItemDatas().ItemBarIcon);

		ItemType = MyItem->GetItemDatas().ItemType;

		ItemName->SetText(FText::FromString(MyItem->GetItemDatas().ItemName));

		


		if (MyItem->GetItemDatas().ItemCurStack == 0)
		{
			//TODO : 근접무기와 부착물을 제외한 아이템은 curStack이 0이 되면 사라지도록 Inven에서 관리하기.
			if (MyItem->GetItemDatas().ItemType == EItemTypes::MELEEWEAPON && MyItem->GetItemDatas().ItemType == EItemTypes::ATTACHMENT)
			{
				ItemStackBlock->SetVisibility(ESlateVisibility::Hidden);
			}
			else
			{
				CachedItem = nullptr;

				SetVisibility(ESlateVisibility::Hidden);
			}
		}
		else
			ItemStackBlock->SetText(FText::AsNumber(MyItem->GetItemDatas().ItemCurStack));
		//AddToViewport();
		//SetVisibility(ESlateVisibility::Visible);

	}
}

void UC_BasicItemBarWidget::SetPercent(float curTime, float endTime)
{
	ItemUsingTimer->SetPercent(curTime / endTime);
}
