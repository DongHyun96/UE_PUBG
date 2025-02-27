// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/Panel/C_BasicPanelWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"
#include "Character/Component/C_InvenSystem.h"

#include "InvenUserInterface/C_DragDropOperation.h"

#include "Item/C_Item.h"

#include "Utility/C_Util.h"

bool UC_BasicPanelWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UC_Util::Print("Dropped Item");
	//Cast<UC_DragDropOperation>(InOperation)
	UC_DragDropOperation* MyOperation = Cast<UC_DragDropOperation>(InOperation);

	AC_Item* DroppedItem = Cast<AC_Item>(MyOperation->Payload);

	if (!DroppedItem) return false;

	//bool bIsCtrlDown = InDragDropEvent.IsControlDown();

	if (InDragDropEvent.IsControlDown() && DroppedItem->GetItemDatas().ItemCurStack > 1)
	{
		//OwnerPlayer->GetInvenSystem()->GetInvenUI()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ShowDividePanelWidget(DroppedItem);
		return true;
	}

	switch (DroppedItem->GetItemDatas().ItemType)
	{
	case EItemTypes::NONE:
	case EItemTypes::BULLET:
	case EItemTypes::CONSUMPTIONITEM:
		//return false;
	case EItemTypes::BACKPACK:
	case EItemTypes::MAINGUN:
	case EItemTypes::MELEEWEAPON:
	case EItemTypes::THROWABLE:
	case EItemTypes::ATTACHMENT:
	case EItemTypes::VEST:
	case EItemTypes::HELMET:
		HandleDrop(DroppedItem);
		//DroppedItem->MoveToSlot(OwnerPlayer);
		OwnerPlayer->GetInvenSystem()->GetInvenUI()->UpdateWidget();
		return true;
	default:
		return false;
	}
}

bool UC_BasicPanelWidget::HandleDrop(AC_Item* DroppedItem)
{
	return false;
}

void UC_BasicPanelWidget::ShowDividePanelWidget(AC_Item* DividedItem)
{
}






