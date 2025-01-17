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
		//��Ŭ�� �̺�Ʈ ����
		//TODO : ����
		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UC_BasicItemBarWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		//�巡�� ����
		FEventReply RePlyResult =
			UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
		//TODO : ����


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
			//TODO : ��������� �������� ������ �������� curStack�� 0�� �Ǹ� ��������� Inven���� �����ϱ�.
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
