// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/Panel/ItemPanel/C_InventoryItemPanelWidget.h"
#include "Utility/C_Util.h"


bool UC_InventoryItemPanelWidget::HandleDrop(AC_Item* DroppedItem)
{
    UC_Util::Print("MoveToInven");
    return DroppedItem->MoveToInven(OwnerPlayer);
}


