// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/EquipmentSlot/C_VestSlotWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "Components/ProgressBar.h"

//FReply UC_VestSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
//	{
//		if (AC_EquipableItem* SlotItem = OwnerPlayer->GetInvenComponent()->GetEquipmentItems()[EEquipSlot::VEST])
//		{   // 우클릭 이벤트 실행
//			//EquippedItem->Interaction(OwnerCharacter);
//			SlotItem->MoveToAround(OwnerPlayer);
//			//InitInvenUIWidget();
//
//			//NativeOnListItemObjectSet에서의 호출과 중복으로 일단 주석처리, 다만 이벤트시에 초기화가 필요하면 사용해야 할 수 있음.
//			//if (!CachedItem) return;
//
//			//InitBar(CachedItem);
//			//SetVisibility(ESlateVisibility::Visible);
//			//UpdateWidget();
//
//			if (UC_InventoryUIWidget* InvenUiWidget = GetTypedOuter<UC_InventoryUIWidget>())
//				InvenUiWidget->UpdateWidget();
//			return FReply::Handled();
//		}
//	}
//	else
//	{
//		//UC_Util::Print("No cached item to interact with!", FColor::Red, 5.0f);
//	}
//	// 다른 버튼 클릭 처리
//	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
//}
//
//void UC_VestSlotWidget::UpdateWidget()
//{
//	AC_EquipableItem* SlotItem = OwnerPlayer->GetInvenComponent()->GetEquipmentItems()[EEquipSlot::VEST];
//	
//	UpdateSlotItemImage(SlotItem);
//}

void UC_VestSlotWidget::UpdateDurabilityBar(float percent)
{
	DurabilityBar->SetPercent(1.0f - percent);
	DurabilityBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
