// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/Panel/ItemPanel/EquipmentPanel/C_EquipmentPanelWdiget.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_MainGunSlotWidget.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_SubGunSlotWidget.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/C_MeleeWeaponSlotWidget.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/C_ThrowableWeaponSlotWidget.h"

#include "InvenUI/BasicItemSlot/EquipmentSlot/C_EquipmentSlotWidget.h"
#include "InvenUI/BasicItemSlot/EquipmentSlot/C_HelmetSlotWidget.h"
#include "InvenUI/BasicItemSlot/EquipmentSlot/C_VestSlotWidget.h"
#include "InvenUI/BasicItemSlot/EquipmentSlot/C_BackPackSlotWidget.h"

#include "InvenUI/C_InventoryUIWidget.h"
#include "Character/Component/C_InvenSystem.h"

#include "InvenUserInterface/C_DragDropOperation.h"

#include "Item/C_Item.h"

#include "Utility/C_Util.h"

bool UC_EquipmentPanelWdiget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	// �ڽ� ������ ����� ó���ϵ��� �켱 ȣ��
	if (SubGunSlot && SubGunSlot->IsHovered())
	{
		bool bChildHandled = SubGunSlot->NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
		if (bChildHandled)
		{
			// �ڽ� ������ ����� ó�������� �θ�� ó������ ����
			if (UC_InventoryUIWidget* InvenUiWidget = GetTypedOuter<UC_InventoryUIWidget>())
				InvenUiWidget->UpdateWidget();
			return true;
		}
	}

	// �θ� �������� ��� ó��
	UE_LOG(LogTemp, Warning, TEXT("Item dropped on EquipmentPanel"));
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation); // �θ� �������� ��� ó�� �Ϸ�
}

void UC_EquipmentPanelWdiget::UpdateWidget()
{
	if (MainGunSlot)
		MainGunSlot->UpdateWidget();

	if (SubGunSlot)
		SubGunSlot->UpdateWidget();

	if (MeleeSlot)
		MeleeSlot->UpdateWidget();

	if (ThrowableSlot)
		ThrowableSlot->UpdateWidget();

	if (HelmetSlot)
		HelmetSlot->UpdateWidget();

	if (BackPackSlot)
		BackPackSlot->UpdateWidget();

	if (VestSlot)
		VestSlot->UpdateWidget();

}

void UC_EquipmentPanelWdiget::SetOwnerPlayer(AC_Player* InOwnerPlayer)
{
	OwnerPlayer = InOwnerPlayer;

	MainGunSlot->SetOwnerPlayer(InOwnerPlayer);

	SubGunSlot->SetOwnerPlayer(InOwnerPlayer);

	MeleeSlot->SetOwnerPlayer(InOwnerPlayer);

	ThrowableSlot->SetOwnerPlayer(InOwnerPlayer);

	HelmetSlot->SetOwnerPlayer(InOwnerPlayer);

	BackPackSlot->SetOwnerPlayer(InOwnerPlayer);

	VestSlot->SetOwnerPlayer(InOwnerPlayer);

}

bool UC_EquipmentPanelWdiget::HandleDrop(AC_Item* DroppedItem)
{
	UC_Util::Print("MoveToSlot");
	return DroppedItem->MoveToSlot(OwnerPlayer);
}
