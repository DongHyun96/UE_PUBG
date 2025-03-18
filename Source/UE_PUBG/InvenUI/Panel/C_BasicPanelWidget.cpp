// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/Panel/C_BasicPanelWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"
#include "Character/Component/C_InvenSystem.h"

#include "InvenUserInterface/C_DragDropOperation.h"

#include "Item/C_Item.h"
#include "Item/C_ItemBox.h"

#include "Utility/C_Util.h"

bool UC_BasicPanelWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UC_Util::Print("Dropped Item");
	//Cast<UC_DragDropOperation>(InOperation)
	UC_DragDropOperation* MyOperation = Cast<UC_DragDropOperation>(InOperation);

	//AC_Item* DroppedItem = Cast<AC_Item>(MyOperation->Payload);

	UC_ItemBox* DroppedItemBox = MyOperation->DraggedItemBox;

	UC_Util::Print("Create DroppedItemBox");

	if (!DroppedItemBox) return false;

	UC_Util::Print("Success DroppedItemBox");

	//bool bIsCtrlDown = InDragDropEvent.IsControlDown();

	if (InDragDropEvent.IsControlDown() && DroppedItemBox->GetItemStackCount() > 1)
	{
		//OwnerPlayer->GetInvenSystem()->GetInvenUI()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		OwnerPlayer->GetInvenSystem()->GetInvenUI()->MainPanelsSetVisivility(ESlateVisibility::HitTestInvisible);
		ShowDividePanelWidget(DroppedItemBox);
		return true;
	}

	switch (DroppedItemBox->GetItemRef()->GetItemDatas()->ItemType)
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
		HandleDrop(DroppedItemBox);
		//DroppedItem->MoveToSlot(OwnerPlayer);
		OwnerPlayer->GetInvenSystem()->GetInvenUI()->UpdateWidget();
		return true;
	default:
		return false;
	}
}

bool UC_BasicPanelWidget::HandleDrop(UC_ItemBox* DroppedItemBox)
{
	return false;
}

void UC_BasicPanelWidget::ShowDividePanelWidget(UC_ItemBox* DividedItemBox)
{
}






