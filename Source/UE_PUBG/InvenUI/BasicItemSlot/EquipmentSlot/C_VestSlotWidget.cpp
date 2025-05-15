// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/EquipmentSlot/C_VestSlotWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "Components/ProgressBar.h"

void UC_VestSlotWidget::UpdateDurabilityBar(float percent)
{
	DurabilityBar->SetPercent(1.0f - percent);
	DurabilityBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
