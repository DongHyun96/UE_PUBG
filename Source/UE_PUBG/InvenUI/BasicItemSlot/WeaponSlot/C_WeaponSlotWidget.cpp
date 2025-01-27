// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/WeaponSlot/C_WeaponSlotWidget.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenSystem.h"

#include "Components/CanvasPanel.h"

#include "InvenUserInterface/C_DragDropOperation.h"


FReply UC_WeaponSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// ��Ŭ������ üũ
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		AC_Weapon* SlotItem = OwnerPlayer->GetEquippedComponent()->GetWeapons()[WeaponType];

		if (SlotItem)
		{  
			// ��Ŭ�� �̺�Ʈ ����
			// slot������ ������������ �����ϸ� �����ϴ� �Լ��� �����Ѵ�.
			MouseRBDownInteraction(SlotItem);
			//{
			//	OwnerPlayer->GetEquippedComponent()->SetSlotWeapon(WeaponType, nullptr);
			//}

			if (UC_InventoryUIWidget* InvenUiWidget = GetTypedOuter<UC_InventoryUIWidget>())
				InvenUiWidget->UpdateWidget();
			return FReply::Handled();
		}
	}
	// �ٸ� ��ư Ŭ�� ó��
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UC_WeaponSlotWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		AC_Weapon* SlotItem = OwnerPlayer->GetEquippedComponent()->GetWeapons()[WeaponType];

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
	return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
}

void UC_WeaponSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	//dragdrop class�� ���� ����� ����ؾ� �� �� ����.
	UC_DragDropOperation* DragOperation = NewObject<UC_DragDropOperation>();
	//if (!CachedItem) return;
	//UObject* ResourceObject = Cast<UImage>(CachedItem->GetItemDatas().ItemIcon)->Brush.GetResourceObject();//UTexture2D�ε� �Ʒ��ٿ��� �ٷ� ��� ������ �� ������?
	UTexture2D* Texture = nullptr;

	AC_Item* SlotItem = OwnerPlayer->GetEquippedComponent()->GetWeapons()[WeaponType];

	if (!SlotItem) return;

	Texture = Cast<UTexture2D>(SlotItem->GetItemDatas().ItemBarIcon);

	UBorder* Border = NewObject<UBorder>();
	FLinearColor BorderColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.1f); // (R, G, B, A)
	Border->SetBrushColor(BorderColor);

	//Border->SetPadding(FMargin(2.0f)); �е��� �ʿ��ϸ� ����ϱ�.

	UImage* DragVisual = NewObject<UImage>(Texture);

	DragVisual->SetBrushFromTexture(Texture);
	//DragVisual->SetBrushFromTexture(Texture);
	DragVisual->Brush.ImageSize = FVector2D(64.f, 64.f);
	Border->SetContent(DragVisual);

	DragOperation->DefaultDragVisual = Border;

	//DragOperation->DefaultDragVisual = ItemImage1; // �巡�� �� �������� �̸����� �̹���
	//DragOperation->DefaultDragVisual = this; // �巡�� �� �������� �̸����� �̹���

	DragOperation->Payload = SlotItem; // �巡�� �� ������ ������ (������)
	DragOperation->Pivot = EDragPivot::MouseDown;
	//DragOperation->Pivot = EDragPivot::CenterCenter;

	DragOperation->DraggedItem = SlotItem;

	DragOperation->curWeaponSlot = WeaponType;

	this->Visibility = ESlateVisibility::SelfHitTestInvisible;
	OwnerPlayer->GetInvenSystem()->GetInvenUI()->SetIsDragging(true);

	//OwnerCharacter->GetInvenSystem()->GetInvenUI()->SetItemListZorder(CachedItem->GetOwnerCharacter());

	OutOperation = DragOperation;
}

void UC_WeaponSlotWidget::UpdateWidget()
{
	if (!OwnerPlayer) return;

	AC_Weapon* CachedWeapon = OwnerPlayer->GetEquippedComponent()->GetWeapons()[WeaponType];
	//TODO : Gun�� ��� ����ϴ� �Ѿ��� ������ ������ ǥ������. ���� Ŭ�������� ������ ����.

	UpdateSlotItemImage(CachedWeapon);
}

void UC_WeaponSlotWidget::UpdateSlotItemImage(AC_Weapon* SlotItem)
{
	if (SlotItem)
	{
		WeaponImage->SetBrushFromTexture(SlotItem->GetItemDatas().ItemSlotImage);
		WeaponName->SetText(FText::FromString(SlotItem->GetItemDatas().ItemName));
		FSlateBrush Brush = WeaponImage->GetBrush();
		Brush.TintColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
		WeaponImage->SetBrush(Brush);
		WeaponSlotPanel->SetVisibility(ESlateVisibility::Visible);
		//SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		WeaponImage->SetBrushFromTexture(nullptr);
		FSlateBrush Brush = WeaponImage->GetBrush();
		Brush.TintColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.3f);
		WeaponImage->SetBrush(Brush);
		WeaponSlotPanel->SetVisibility(ESlateVisibility::Hidden); //TODO : Hidden�� Collaps? �̰� ���� �����ϱ�.
	}
}

bool UC_WeaponSlotWidget::MouseRBDownInteraction(AC_Weapon* inSlotWeapon)
{
	return false;
}
