// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_SubGunSlotWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"
#include "Item/Weapon/Gun/C_Gun.h"

//FReply UC_SubGunSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//	// ��Ŭ������ üũ
//	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
//	{
//		AC_Weapon* SlotItem = OwnerPlayer->GetEquippedComponent()->GetWeapons()[EWeaponSlot::SUB_GUN];
//		if (SlotItem)
//		{   // ��Ŭ�� �̺�Ʈ ����
//			if (SlotItem->MoveToAround(OwnerPlayer))
//			{
//				OwnerPlayer->GetEquippedComponent()->SetSlotWeapon(EWeaponSlot::SUB_GUN, nullptr);
//			}
//
//			if (UC_InventoryUIWidget* InvenUiWidget = GetTypedOuter<UC_InventoryUIWidget>())
//				InvenUiWidget->UpdateWidget();
//			return FReply::Handled();
//		}
//	}
//	// �ٸ� ��ư Ŭ�� ó��
//	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
//}
//
//void UC_SubGunSlotWidget::UpdateWidget()
//{
//	AC_Weapon* SlotItem = OwnerPlayer->GetEquippedComponent()->GetWeapons()[EWeaponSlot::SUB_GUN];
//
//	UpdateSlotItemImage(SlotItem);
//}


bool UC_SubGunSlotWidget::ChangedGunSlot(AC_Weapon* gun)
{
	if (!gun) return false;
	UC_EquippedComponent* EquipComp = OwnerPlayer->GetEquippedComponent();

	EquipComp->SetSlotWeapon(EWeaponSlot::MAIN_GUN, nullptr);//�켱 �巡�׵� ������ ������ ������ ����.

	AC_Weapon* curSlotGun = EquipComp->SetSlotWeapon(WeaponType, gun);//����� ���Կ� �巡�׵� �������� ����

	if (IsValid(EquipComp->SetSlotWeapon(EWeaponSlot::MAIN_GUN, curSlotGun)))//����� ���Կ� �����Ǿ� �־��� �������� �巡�׵� ������ ���Կ� ����.
		return true;
	else
		return false;
}	
