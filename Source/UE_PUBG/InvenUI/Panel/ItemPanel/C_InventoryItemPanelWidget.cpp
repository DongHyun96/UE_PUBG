// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/Panel/ItemPanel/C_InventoryItemPanelWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"
#include "InvenUI/Panel/DivideItemPanel/C_BasicDivideItemPanelWidget.h"
#include "Character/C_Player.h"
#include "Character/Component/C_InvenSystem.h"
#include "Utility/C_Util.h"


bool UC_InventoryItemPanelWidget::HandleDrop(AC_Item* DroppedItem)
{
    return DroppedItem->MoveToInven(OwnerPlayer, DroppedItem->GetItemCurStack());
}

void UC_InventoryItemPanelWidget::ShowDividePanelWidget(AC_Item* DroppedItem)
{
    UC_BasicDivideItemPanelWidget* DivideItemDropWidget = GetOwnerPlayer()->GetInvenSystem()->GetInvenUI()->GetDivideItemInventoryDropWidget();

    DivideItemDropWidget->SetCachedItem(DroppedItem);

    DivideItemDropWidget->UpdateWidget();

    DivideItemDropWidget->SetVisibility(ESlateVisibility::Visible);
}

