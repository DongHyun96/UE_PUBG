// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/WeaponSlot/C_MeleeWeaponSlotWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"

//FReply UC_MeleeWeaponSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//	// 우클릭인지 체크
//	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
//	{
//		AC_Weapon* SlotItem = OwnerPlayer->GetEquippedComponent()->GetWeapons()[EWeaponSlot::MELEE_WEAPON];
//		if (SlotItem)
//		{   // 우클릭 이벤트 실행
//			if (SlotItem->MoveToAround(OwnerPlayer))
//			{
//				OwnerPlayer->GetEquippedComponent()->SetSlotWeapon(EWeaponSlot::MELEE_WEAPON, nullptr);
//			}
//
//			if (UC_InventoryUIWidget* InvenUiWidget = GetTypedOuter<UC_InventoryUIWidget>())
//				InvenUiWidget->UpdateWidget();
//			return FReply::Handled();
//		}
//	}
//	// 다른 버튼 클릭 처리
//	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
//}
//
//void UC_MeleeWeaponSlotWidget::UpdateWidget()
//{
//	AC_Weapon* SlotItem = OwnerPlayer->GetEquippedComponent()->GetWeapons()[EWeaponSlot::MELEE_WEAPON];
//
//	UpdateSlotItemImage(SlotItem);
//}

bool UC_MeleeWeaponSlotWidget::MouseRBDownInteraction(AC_Weapon* inSlotWeapon)
{
	return inSlotWeapon->MoveToInven(OwnerPlayer);
}


