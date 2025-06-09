// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/Panel/ItemPanel/C_AroundItemPanelWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"
#include "InvenUI/Panel/DivideItemPanel/C_BasicDivideItemPanelWidget.h"
#include "Character/C_Player.h"
#include "Character/Component/C_InvenSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/C_Util.h"
bool UC_AroundItemPanelWidget::HandleDrop(AC_Item* DroppedItem)
{
    if (DroppedItem->MoveToAround(OwnerPlayer, DroppedItem->GetItemCurStack()))
    {
        UGameplayStatics::PlaySound2D(DroppedItem, DroppedItem->GetPickUpSound());
        return true;
    }
    return false;
}

void UC_AroundItemPanelWidget::ShowDividePanelWidget(AC_Item* DroppedItem)
{
    UC_BasicDivideItemPanelWidget* DivideItemDropWidget = GetOwnerPlayer()->GetInvenSystem()->GetInvenUI()->GetDivideItemAroundDropWidget();

    DivideItemDropWidget->SetCachedItem(DroppedItem);

    DivideItemDropWidget->UpdateWidget();

    DivideItemDropWidget->SetVisibility(ESlateVisibility::Visible);
}

