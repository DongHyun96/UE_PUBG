// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/Panel/ItemPanel/C_AroundItemPanelWidget.h"
#include "Utility/C_Util.h"
bool UC_AroundItemPanelWidget::HandleDrop(AC_Item* DroppedItem)
{
    UC_Util::Print("MoveToAround");
    return DroppedItem->MoveToAround(OwnerPlayer);
}
