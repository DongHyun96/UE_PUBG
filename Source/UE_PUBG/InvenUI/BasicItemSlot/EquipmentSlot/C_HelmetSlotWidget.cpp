// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/EquipmentSlot/C_HelmetSlotWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "Components/ProgressBar.h"

void UC_HelmetSlotWidget::UpdateDurabilityBar(float percent)
{
	DurabilityBar->SetPercent(1.0f - percent);
}
