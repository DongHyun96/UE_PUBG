// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/EquipmentSlot/C_AttachableItemSlotWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"

#include "Character/C_Player.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenSystem.h"

#include "Components/Image.h"
#include "Components/Border.h"

#include "InvenUserInterface/C_DragDropOperation.h"

#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Attachment/C_AttachableItem.h"

FReply UC_AttachableItemSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		AC_Gun* curGun = Cast<AC_Gun>(OwnerPlayer->GetEquippedComponent()->GetWeapons()[GunSlot]);

		if (!curGun) return FReply::Unhandled();

		AC_AttachableItem* SlotItem = curGun->GetAttachableItem()[SlotName];

		if (SlotItem)
		{
			//�巡�� �̺�Ʈ ����.
			//�巡�׸� �����ϰ� ������
			FEventReply RePlyResult =
				UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);

			//UC_Util::Print("LeftMouseButton");
			OwnerPlayer->GetInvenSystem()->GetInvenUI()->SetIsDragging(true);

			return RePlyResult.NativeReply;
		}
	}
	else if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		AC_Gun* curGun = Cast<AC_Gun>(OwnerPlayer->GetEquippedComponent()->GetWeapons()[GunSlot]);

		if (!curGun) return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

		if (AC_AttachableItem* SlotItem = curGun->GetAttachableItem()[SlotName])
		{   // ��Ŭ�� �̺�Ʈ ����
			//EquippedItem->Interaction(OwnerCharacter);
			SlotItem->MoveToInven(OwnerPlayer);
			//InitInvenUIWidget();

			//NativeOnListItemObjectSet������ ȣ��� �ߺ����� �ϴ� �ּ�ó��, �ٸ� �̺�Ʈ�ÿ� �ʱ�ȭ�� �ʿ��ϸ� ����ؾ� �� �� ����.
			//if (!CachedItem) return;

			//InitBar(CachedItem);
			//SetVisibility(ESlateVisibility::Visible);
			//UpdateWidget();

			if (UC_InventoryUIWidget* InvenUiWidget = GetTypedOuter<UC_InventoryUIWidget>())
				InvenUiWidget->UpdateWidget();
			return FReply::Handled();
		}
	}
	// �ٸ� ��ư Ŭ�� ó��
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

//FReply UC_AttachableItemSlotWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
//	{
//		//AC_EquipableItem* SlotItem = OwnerPlayer->GetInvenComponent()->GetEquipmentItems()[EquipSlot];
//		//AC_AttachableItem* SlotItem = Cast<AC_Gun>(OwnerPlayer->GetEquippedComponent()->GetWeapons()[GunSlot])->GetAttachableItem()[SlotName];
//
//		AC_Gun* curGun = Cast<AC_Gun>(OwnerPlayer->GetEquippedComponent()->GetWeapons()[GunSlot]);
//
//		if (!curGun) return FReply::Unhandled();
//
//		AC_AttachableItem* SlotItem = curGun->GetAttachableItem()[SlotName];
//
//		if (SlotItem)
//		{
//			//�巡�� �̺�Ʈ ����.
//			//�巡�׸� �����ϰ� ������
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

void UC_AttachableItemSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	//dragdrop class�� ���� ����� ����ؾ� �� �� ����.
	UC_DragDropOperation* DragOperation = NewObject<UC_DragDropOperation>();
	//if (!CachedItem) return;
	//UObject* ResourceObject = Cast<UImage>(CachedItem->GetItemDatas().ItemIcon)->Brush.GetResourceObject();//UTexture2D�ε� �Ʒ��ٿ��� �ٷ� ��� ������ �� ������?
	UTexture2D* Texture = nullptr;

	AC_Gun* curGun = Cast<AC_Gun>(OwnerPlayer->GetEquippedComponent()->GetWeapons()[GunSlot]);
	
	if (!curGun) return;

	AC_AttachableItem* SlotItem = curGun->GetAttachableItem()[SlotName];

	if (!SlotItem) return;

	Texture = Cast<UTexture2D>(SlotItem->GetItemDatas().ItemBarIcon);

	UBorder* Border = NewObject<UBorder>();
	FLinearColor BorderColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.1f); // (R, G, B, A)
	Border->SetBrushColor(BorderColor);

	UImage* DragVisual = NewObject<UImage>(Texture);

	DragVisual->SetBrushFromTexture(Texture);
	DragVisual->Brush.ImageSize = FVector2D(64.f, 64.f);
	Border->SetContent(DragVisual);

	DragOperation->DefaultDragVisual = Border;
	DragOperation->Payload = SlotItem; // �巡�� �� ������ ������ (������)
	DragOperation->Pivot = EDragPivot::MouseDown;

	FVector2D MousePosition = InMouseEvent.GetScreenSpacePosition();
	// ���� ���콺 Ŭ�� ��ġ �������� (ȭ�� ��ǥ)
	FVector2D Offset = DragVisual->Brush.ImageSize * 0.5f;
	FVector2D CenteredPosition = MousePosition - Offset;

	// ���� ����(ItemBar)�� ȭ�� ��ǥ ��������
	FVector2D WidgetScreenPosition = InGeometry.AbsoluteToLocal(CenteredPosition); //�� �̰� ��߸� �ϴ°�?

	// �巡�� ���־� ��ġ�� ������ ���� (������ �������� ����)
	Border->SetRenderTranslation(WidgetScreenPosition);

	DragOperation->DraggedItem = SlotItem;

	DragOperation->curWeaponSlot = SlotItem->GetOwnerGun()->GetWeaponSlot();

	this->Visibility = ESlateVisibility::SelfHitTestInvisible;
	OwnerPlayer->GetInvenSystem()->GetInvenUI()->SetIsDragging(true);

	OutOperation = DragOperation;
}

void UC_AttachableItemSlotWidget::UpdateSlotItemImage(AC_Gun* SlotItem)
{
	AC_AttachableItem* curItem = SlotItem->GetAttachableItem()[SlotName];

	//AC_AttachableItem* curItem = Cast<AC_Gun>(OwnerPlayer->GetEquippedComponent()->GetWeapons()[GunSlot])->GetAttachableItem()[SlotName];

	if (curItem)
	{
		ItemImage->SetBrushFromTexture(curItem->GetItemDatas().ItemSlotImage);
		FSlateBrush Brush = ItemImage->GetBrush();
		Brush.TintColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
		ItemImage->SetBrush(Brush);
		//SetVisibility(ESlateVisibility::Visible); //TODO : �ʿ��� ����ΰ�? ������ ����ΰ�? üũ�ϰ� �����ϱ�.
	}
	else
	{
		ItemImage->SetBrushFromTexture(nullptr);
		FSlateBrush Brush = ItemImage->GetBrush();
		Brush.TintColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.3f);
		ItemImage->SetBrush(Brush);
	}
}

void UC_AttachableItemSlotWidget::DiscriminateSlot(TArray<EPartsName> AttachableSlots, AC_Gun* SlotItem)
{
	for (EPartsName Name : AttachableSlots)
	{
		if (SlotName == Name)
		{
			this->UpdateSlotItemImage(SlotItem);
			this->SetVisibility(ESlateVisibility::Visible); //Widget Ȱ��ȭ.
			return;
		}
	}
	this->SetVisibility(ESlateVisibility::Collapsed); // Widget ��Ȱ��ȭ.
}

//void UC_AttachableItemSlotWidget::DiscriminateSlot(TArray<EPartsName> AttachableSlots, AC_Gun SlotItem)
//{
//	for (EPartsName Name : AttachableSlots)
//	{
//		if (SlotName == Name)
//		{
//			this->UpdateSlotItemImage(SlotItem);
//			this->SetVisibility(ESlateVisibility::Visible); //Widget Ȱ��ȭ.
//			return;
//		}
//	}
//	this->SetVisibility(ESlateVisibility::Collapsed); // Widget ��Ȱ��ȭ.
//}
