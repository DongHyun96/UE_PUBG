// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_MainGunSlotWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"
#include "Item/Weapon/Gun/C_Gun.h"

//FReply UC_MainGunSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//	// 우클릭인지 체크
//	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
//	{
//		AC_Weapon* SlotItem = OwnerPlayer->GetEquippedComponent()->GetWeapons()[EWeaponSlot::MAIN_GUN];
//		if (SlotItem)
//		{   // 우클릭 이벤트 실행
//			if (SlotItem->MoveToAround(OwnerPlayer))
//			{
//				OwnerPlayer->GetEquippedComponent()->SetSlotWeapon(EWeaponSlot::MAIN_GUN, nullptr);
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
//void UC_MainGunSlotWidget::UpdateWidget()
//{
//	//TODO : Gun의 경우 사용하는 총알의 종류와 갯수를 표시해줌. 하위 클래스에서 진행할 예정.
//	AC_Weapon* SlotItem = OwnerPlayer->GetEquippedComponent()->GetWeapons()[EWeaponSlot::MAIN_GUN];
//
//	UpdateSlotItemImage(SlotItem);
//}




bool UC_MainGunSlotWidget::ChangedGunSlot(AC_Gun* gun)
{
	if (!gun) return false;
	UC_EquippedComponent* EquipComp = OwnerPlayer->GetEquippedComponent();

	if (OwnerPlayer->GetHandState() == EHandState::WEAPON_GUN)
		if (EquipComp->SwapSlotsWhileGunHandState()) return true;

	EquipComp->SetSlotWeapon(EWeaponSlot::SUB_GUN, nullptr);//우선 드래그된 아이템 슬롯의 장착을 해제.

	AC_Weapon* curSlotGun = EquipComp->SetSlotWeapon(WeaponType, gun);//드랍된 슬롯에 드래그된 아이템을 장착

	if (IsValid(EquipComp->SetSlotWeapon(EWeaponSlot::SUB_GUN, curSlotGun)))//드랍된 슬롯에 장착되어 있었던 아이템을 드래그된 아이템 슬롯에 장착.
		return true;
	else
		return false;
}
