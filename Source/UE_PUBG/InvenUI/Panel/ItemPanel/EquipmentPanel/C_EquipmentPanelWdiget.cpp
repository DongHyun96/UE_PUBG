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
	// 자식 위젯이 드롭을 처리하도록 우선 호출
	if (SubGunSlot && SubGunSlot->IsHovered())
	{
		bool bChildHandled = SubGunSlot->NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
		if (bChildHandled)
		{
			// 자식 위젯이 드롭을 처리했으면 부모는 처리하지 않음
			if (UC_InventoryUIWidget* InvenUiWidget = GetTypedOuter<UC_InventoryUIWidget>())
				InvenUiWidget->UpdateWidget();
			return true;
		}
	}

	// 부모 위젯에서 드롭 처리
	UE_LOG(LogTemp, Warning, TEXT("Item dropped on EquipmentPanel"));
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation); // 부모 위젯에서 드롭 처리 완료
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

bool UC_EquipmentPanelWdiget::HandleDrop(AC_Item* DroppedItemBox)
{
	UC_Util::Print("MoveToSlot");
	return DroppedItemBox->MoveToSlot(OwnerPlayer,DroppedItemBox->GetItemCurStack());
}


