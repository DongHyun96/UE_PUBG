// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/Panel/ItemPanel/C_InventoryItemPanelWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"
#include "InvenUI/Panel/DivideItemPanel/C_BasicDivideItemPanelWidget.h"
#include "Character/C_Player.h"
#include "Character/Component/C_InvenSystem.h"
#include "Utility/C_Util.h"


bool UC_InventoryItemPanelWidget::HandleDrop(UC_ItemBox* DroppedItemBox)
{
    UC_Util::Print("MoveToInven");
    return DroppedItemBox->GetItemRef()->MoveToInven(OwnerPlayer, DroppedItemBox->GetItemStackCount());
}

void UC_InventoryItemPanelWidget::ShowDividePanelWidget(UC_ItemBox* DividedItemBox)
{
    UC_BasicDivideItemPanelWidget* DivideItemDropWidget = GetOwnerPlayer()->GetInvenSystem()->GetInvenUI()->GetDivideItemInventoryDropWidget();

    DivideItemDropWidget->SetCachedItem(DividedItemBox);

    DivideItemDropWidget->UpdateWidget();

    DivideItemDropWidget->SetVisibility(ESlateVisibility::Visible);
}

