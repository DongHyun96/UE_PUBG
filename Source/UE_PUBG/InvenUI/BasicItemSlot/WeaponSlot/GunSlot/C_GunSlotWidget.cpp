// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_GunSlotWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"
#include "InvenUI/BasicItemSlot/EquipmentSlot/C_AttachableItemSlotWidget.h"
#include "Character/Component/C_InvenSystem.h"

#include "InvenUserInterface/C_DragDropOperation.h"
//#include "C_DragDropOperation.h"

#include "Item/C_Item.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Attachment/C_AttachableItem.h"
#include "Kismet/GameplayStatics.h"
#include "Singleton/C_GameInstance.h"
#include "Engine/DataTable.h"
#include "Utility/C_Util.h"

void UC_GunSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeAttachSlotMap();
}


//FReply UC_GunSlotWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
//	{
//		//for (auto& AttachSlot : AttachSlotWidgets)
//		//{
//		//	if (UC_AttachableItemSlotWidget* PartSlot = AttachSlot.Value)
//		//	{
//		//		// 자식 위젯의 이벤트 처리 결과 확인
//		//		FReply ChildReply = PartSlot->NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
//		//
//		//		if (ChildReply.IsEventHandled()) // 자식 위젯이 이벤트를 처리한 경우
//		//		{
//		//			return ChildReply; // 처리된 결과 반환
//		//		}
//		//	}
//		//}sd
//		return FReply::Unhandled();
//
//		AC_Weapon* SlotItem = OwnerPlayer->GetEquippedComponent()->GetWeapons()[WeaponType];
//
//		if (SlotItem)
//		{
//			//드래그 이벤트 실행.
//			//드래그를 시작하고 반응함
//			FEventReply RePlyResult =
//				UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
//
//			//UC_Util::Print("LeftMouseButton");
//			OwnerPlayer->GetInvenSystem()->GetInvenUI()->SetIsDragging(true);
//
//			return RePlyResult.NativeReply;
//		}
//	}
//	return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
//}

bool UC_GunSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UC_Util::Print("Dropped Item");
	//Cast<UC_DragDropOperation>(InOperation)
	UC_DragDropOperation* MyOperation = Cast<UC_DragDropOperation>(InOperation);
	AC_Item* DroppedItem = Cast<AC_Item>(MyOperation->Payload);
	//InOperation->
	if (!DroppedItem) return false;

	HandleDrop(MyOperation);
	OwnerPlayer->GetInvenSystem()->GetInvenUI()->UpdateWidget();
	return true;
}

bool UC_GunSlotWidget::MouseRBDownInteraction(AC_Weapon* inSlotWeapon)
{

	if (OwnerPlayer->GetIsActivatingConsumableItem()) return false;

	if (inSlotWeapon->MoveToAround(OwnerPlayer, inSlotWeapon->GetItemCurStack()))
	{
		UC_Util::Print("GunSlotRBDownInteraction", FColor::Red, 10.f);
		UGameplayStatics::PlaySound2D(inSlotWeapon, inSlotWeapon->GetPickUpSound());
		return true;	
	}
    return false;
}

void UC_GunSlotWidget::UpdateWidget()
{
	Super::UpdateWidget();

	UpdateAttachableSlotVisibility();

	UC_EquippedComponent* EquipComp = OwnerPlayer->GetEquippedComponent();
	


	AC_Gun* curWeapon = Cast<AC_Gun>(EquipComp->GetWeapons()[WeaponType]);

	FName BulletTypeName = NAME_None;

	UC_GameInstance* GameInstance = Cast<UC_GameInstance>(GetGameInstance());

	UDataTable* ItemDataTable = nullptr;

	FItemData* Row = nullptr;

	UTexture2D* ItemSlotImage = nullptr;

	FString ItemName;

	if (curWeapon)
	{
		BulletTypeName = curWeapon->GetCurrentBulletTypeName();

		ItemDataTable = *GameInstance->GetDataTables().Find(EDataTableType::Item);
		Row = ItemDataTable->FindRow<FItemData>(BulletTypeName, TEXT("Looking for item data"));
		ItemSlotImage = Row->ItemSlotImage;
		ItemName = Row->ItemName;
	}

	UpdateCurAmmo(curWeapon);

	UpdateLeftAmmo(curWeapon);

	UpdateAmmoImage(curWeapon, ItemSlotImage);

	UpdateBulletTypeText(curWeapon, ItemName);


	//if (CurAmmo)
	//{
	//	if (curWeapon)
	//	{
	//		CurAmmo->SetText(FText::FromString(FString::Printf(TEXT("%d"), curWeapon->GetCurBulletCount())));
	//		CurAmmo->SetVisibility(ESlateVisibility::Visible);
	//	}
	//	else
	//	{
	//		CurAmmo->SetText(FText::FromString(TEXT("0")));
	//		CurAmmo->SetVisibility(ESlateVisibility::Collapsed);
	//	}
	//}

	//if (LeftAmmo)
	//{
	//	if (curWeapon)
	//	{
	//		int curStack = InvenComp->GetTotalStackByItemName(BulletTypeName);

	//		LeftAmmo->SetText(FText::FromString(FString::Printf(TEXT("%d"), curStack)));
	//		LeftAmmo->SetVisibility(ESlateVisibility::Visible);
	//	}
	//	else
	//	{
	//		LeftAmmo->SetText(FText::FromString(TEXT("0")));
	//		LeftAmmo->SetVisibility(ESlateVisibility::Collapsed);
	//	}
	//}

	//if (AmmoImage)
	//{
	//	if (curWeapon)
	//	{
	//		AmmoImage->SetVisibility(ESlateVisibility::Visible);
	//		AmmoImage->SetBrushFromTexture(ItemSlotImage);
	//	}
	//	else
	//	{
	//		AmmoImage->SetVisibility(ESlateVisibility::Collapsed);
	//		AmmoImage->SetBrushFromTexture(nullptr);
	//	}
	//}

	//if (BulletTypeText)
	//{
	//	if (curWeapon)
	//	{
	//		BulletTypeText->SetText(FText::FromString(ItemName));
	//		BulletTypeText->SetVisibility(ESlateVisibility::Visible);
	//	}
	//	else
	//	{
	//		BulletTypeText->SetText(FText::FromString(TEXT("")));
	//		BulletTypeText->SetVisibility(ESlateVisibility::Collapsed);
	//	}
	//}

}

void UC_GunSlotWidget::UpdateAttachableSlotVisibility()
{
	UC_EquippedComponent* EquipComp = OwnerPlayer->GetEquippedComponent();

	AC_Gun* curWeapon = Cast<AC_Gun>(EquipComp->GetWeapons()[WeaponType]);

	if (!curWeapon) return;

	TArray<EPartsName> Parts = curWeapon->GetAttachableParts();
	
	for (auto& AttachSlot : AttachSlotWidgets)
	{
		if (UC_AttachableItemSlotWidget* PartSlot = AttachSlot.Value)
		{
			PartSlot->DiscriminateSlot(Parts, curWeapon);
		}
	}

}

void UC_GunSlotWidget::UpdateCurAmmo(AC_Gun* curWeapon)
{
	if (CurAmmo)
	{
		if (curWeapon)
		{
			CurAmmo->SetText(FText::FromString(FString::Printf(TEXT("%d"), curWeapon->GetCurMagazineBulletCount())));
			CurAmmo->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			CurAmmo->SetText(FText::FromString(TEXT("0")));
			CurAmmo->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UC_GunSlotWidget::UpdateLeftAmmo(AC_Gun* curWeapon)
{
	if (LeftAmmo)
	{
		if (curWeapon)
		{
			UC_InvenComponent* InvenComp = OwnerPlayer->GetInvenComponent();

			FName BulletTypeName = curWeapon->GetCurrentBulletTypeName();

			int curStack = InvenComp->GetTotalStackByItemName(BulletTypeName);

			LeftAmmo->SetText(FText::FromString(FString::Printf(TEXT("%d"), curStack)));
			LeftAmmo->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			LeftAmmo->SetText(FText::FromString(TEXT("0")));
			LeftAmmo->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UC_GunSlotWidget::UpdateAmmoImage(AC_Gun* curWeapon, UTexture2D* InItemSlotImage)
{
	if (AmmoImage)
	{
		if (curWeapon)
		{
			AmmoImage->SetVisibility(ESlateVisibility::Visible);
			AmmoImage->SetBrushFromTexture(InItemSlotImage);
		}
		else
		{
			AmmoImage->SetVisibility(ESlateVisibility::Collapsed);
			AmmoImage->SetBrushFromTexture(nullptr);
		}
	}
}

void UC_GunSlotWidget::UpdateBulletTypeText(AC_Gun* curWeapon, FString InBulletTypeString)
{
	if (BulletTypeText)
	{
		if (curWeapon)
		{
			BulletTypeText->SetText(FText::FromString(InBulletTypeString));
			BulletTypeText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			BulletTypeText->SetText(FText::FromString(TEXT("")));
			BulletTypeText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

bool UC_GunSlotWidget::SetAttachmentSlotOnDrop(AC_Weapon* InSlotWeapon, AC_AttachableItem* InAttachableItem)
{
	AC_Gun* SlotGun = Cast<AC_Gun>(InSlotWeapon);
	
	if (!SlotGun) return false;

	UC_Util::Print(SlotGun->GetItemName());

	EItemPlace InAttachableItemPlace = InAttachableItem->GetItemPlace();



	// 해당 아이템에 대한 부착 슬롯이 없다면 return false;
	if (!SlotGun->GetAttachableItem().Contains(InAttachableItem->GetName())) return false;
	
	if (InAttachableItemPlace == EItemPlace::INVEN)
	{
		OwnerPlayer->GetInvenComponent()->RemoveItemToMyList(InAttachableItem);
	}
	else if (InAttachableItemPlace == EItemPlace::AROUND)
	{
		OwnerPlayer->GetInvenComponent()->RemoveItemToAroundList(InAttachableItem);
	}
	
	AC_AttachableItem* ChangedItem = SlotGun->GetAttachableItem()[InAttachableItem->GetName()];

	if (ChangedItem)
	{
		if (!ChangedItem->MoveToInven(OwnerPlayer, ChangedItem->GetItemCurStack()))
			ChangedItem->MoveToAround(OwnerPlayer, ChangedItem->GetItemCurStack());
	}
	SlotGun->SetAttachableItemSlot(InAttachableItem->GetName(), InAttachableItem);
	UGameplayStatics::PlaySound2D(InAttachableItem, InAttachableItem->GetPickUpSound());
	return true;
}

void UC_GunSlotWidget::SetOwnerPlayer(AC_Player* InOwnerPlayer)
{
	OwnerPlayer = InOwnerPlayer;

	if (WB_MuzzleSlot)
		WB_MuzzleSlot->SetOwnerPlayer(InOwnerPlayer);

	if (WB_MagazineSlot)
		WB_MagazineSlot->SetOwnerPlayer(InOwnerPlayer);

	if (WB_ScopeSlot)
		WB_ScopeSlot->SetOwnerPlayer(InOwnerPlayer);

	if (WB_StockSlot)
		WB_StockSlot->SetOwnerPlayer(InOwnerPlayer);

	if (WB_GripSlot)
		WB_GripSlot->SetOwnerPlayer(InOwnerPlayer);

}

bool UC_GunSlotWidget::HandleDrop(UC_DragDropOperation* InOperation)
{
	UC_EquippedComponent* EquipComp = OwnerPlayer->GetEquippedComponent();
	AC_Weapon* curWeapon = EquipComp->GetWeapons()[WeaponType];//드랍된 슬롯의 Weapon
	//AC_Item* DroppedItem = Cast<AC_Item>(InOperation->DraggedItem);
	AC_Item* DroppedItem = InOperation->DraggedItem;

	if (!curWeapon) //return DroppedItem->MoveToSlot(OwnerPlayer); //드랍된 슬롯에 무기가 없다면 바로 장착
	{
		//드롭된 슬롯에 아이템이 없다면 실행
		if (InOperation->curWeaponSlot == EWeaponSlot::MAIN_GUN || InOperation->curWeaponSlot == EWeaponSlot::SUB_GUN)
		{
			if (InOperation->curWeaponSlot != WeaponType)
			{
				// 빈 슬롯과 Gun 슬롯 Swap하는 상황
				if (OwnerPlayer->GetHandState() == EHandState::WEAPON_GUN)
					if (EquipComp->SwapSlotsWhileGunHandState()) return true;
				EquipComp->SetSlotWeapon(InOperation->curWeaponSlot, nullptr);
			}
			EquipComp->SetSlotWeapon(WeaponType, Cast<AC_Weapon>(DroppedItem));
			//DroppedItem->MoveToSlot(OwnerPlayer);
			return true;
		}
	}
	if (curWeapon == DroppedItem) return false; //드래그된 아이템과 드랍된 슬롯의 아이템이 같은 아이템이라면 return false;
	//드롭된 슬롯에 아이템이 있다면 실행
	// 
	//Around의 아이템과 Slot의 아이템을 교체하는 것과 다른 슬롯으로 아이템을 이동하는 것 구현하기.

	//Around의 아이템과 Slot의 아이템을 교체하는 작업은 간단하게 MoveToSlot으로 처리
	if (DroppedItem->GetItemPlace() == EItemPlace::AROUND || DroppedItem->GetItemPlace() == EItemPlace::INVEN)
	{
		if (DroppedItem->GetItemDatas()->ItemType == EItemTypes::ATTACHMENT)
		{
			

			//드롭된 아이템이 부착물이라면 드롭된 슬롯에 우선 장착.
			if (SetAttachmentSlotOnDrop(curWeapon, Cast<AC_AttachableItem>(DroppedItem))) return true;
		}

		if (DroppedItem->MoveToSlot(OwnerPlayer, DroppedItem->GetItemCurStack()))
		{
			UGameplayStatics::PlaySound2D(DroppedItem, DroppedItem->GetPickUpSound());
			return true;
		}
		return false;
	}

	if (DroppedItem->GetItemDatas()->ItemType == EItemTypes::ATTACHMENT)
	{
		if (InOperation->curWeaponSlot != WeaponType)
		{
			if (Cast<AC_AttachableItem>(DroppedItem)->MoveToSlot(OwnerPlayer, DroppedItem->GetItemCurStack()))
				UGameplayStatics::PlaySound2D(DroppedItem, DroppedItem->GetPickUpSound());
		}
	}



	AC_Gun* Gun = Cast<AC_Gun>(DroppedItem);
	
	if (!Gun) return false;

	return ChangedGunSlot(Gun);
}

bool UC_GunSlotWidget::ChangedGunSlot(AC_Gun* gun)
{
	//자식단계에서 override해서 사용.
	return false;
}

void UC_GunSlotWidget::InitializeAttachSlotMap()
{
	AttachSlotWidgets.Empty();

	// 에디터에서 가져온 위젯을 TMap에 추가
	if (WB_MuzzleSlot)
	{
		AttachSlotWidgets.Add(EPartsName::MUZZLE, WB_MuzzleSlot);
	}

	if (WB_MagazineSlot)
	{
		AttachSlotWidgets.Add(EPartsName::MAGAZINE, WB_MagazineSlot);
	}

	if (WB_ScopeSlot)
	{
		AttachSlotWidgets.Add(EPartsName::SCOPE, WB_ScopeSlot);
	}

	if (WB_StockSlot)
	{
		AttachSlotWidgets.Add(EPartsName::GUNSTOCK, WB_StockSlot);
	}

	if (WB_GripSlot)
	{
		AttachSlotWidgets.Add(EPartsName::GRIP, WB_GripSlot);
	}
}



