// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/EquipmentSlot/C_EquipmentSlotWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_InvenSystem.h"
#include "InvenUserInterface/C_DragDropOperation.h"
//#include "Components/Image.h"
//#include "Components/TextBlock.h"
#include "Components/Border.h"


void UC_EquipmentSlotWidget::NativeConstruct()
{
	SetVisibility(ESlateVisibility::Visible);
}

FReply UC_EquipmentSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		if (AC_EquipableItem* SlotItem = OwnerPlayer->GetInvenComponent()->GetEquipmentItems()[EquipSlot])
		{   // ��Ŭ�� �̺�Ʈ ����
			//EquippedItem->Interaction(OwnerCharacter);
			SlotItem->MoveToAround(OwnerPlayer);
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
	else
	{
		//UC_Util::Print("No cached item to interact with!", FColor::Red, 5.0f);
	}
	// �ٸ� ��ư Ŭ�� ó��
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UC_EquipmentSlotWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		AC_EquipableItem* SlotItem = OwnerPlayer->GetInvenComponent()->GetEquipmentItems()[EquipSlot];

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

void UC_EquipmentSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	//dragdrop class�� ���� ����� ����ؾ� �� �� ����.
	UC_DragDropOperation* DragOperation = NewObject<UC_DragDropOperation>();
	//if (!CachedItem) return;
	//UObject* ResourceObject = Cast<UImage>(CachedItem->GetItemDatas().ItemIcon)->Brush.GetResourceObject();//UTexture2D�ε� �Ʒ��ٿ��� �ٷ� ��� ������ �� ������?
	UTexture2D* Texture = nullptr;

	AC_EquipableItem* SlotItem = OwnerPlayer->GetInvenComponent()->GetEquipmentItems()[EquipSlot];


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
	//DragOperation->Pivot = EDragPivot::MouseDown;
	DragOperation->Pivot = EDragPivot::CenterCenter;

	DragOperation->DraggedItem = SlotItem;

	//DragOperation->curWeaponSlot = WeaponType;

	this->Visibility = ESlateVisibility::SelfHitTestInvisible;
	OwnerPlayer->GetInvenSystem()->GetInvenUI()->SetIsDragging(true);

	//OwnerCharacter->GetInvenSystem()->GetInvenUI()->SetItemListZorder(CachedItem->GetOwnerCharacter());

	OutOperation = DragOperation;
}

void UC_EquipmentSlotWidget::UpdateWidget()
{
	if (!OwnerPlayer) return;
	
	AC_EquipableItem* SlotItem = OwnerPlayer->GetInvenComponent()->GetEquipmentItems()[EquipSlot];

	UpdateSlotItemImage(SlotItem);
	//TODO : Vest�� Helmet�� ������ ǥ�ð� �ʿ���. 2������ ǥ�ð� ����. ���ڿ� ����bar, �̰� ����Ŭ�������� �ϱ�.
}

void UC_EquipmentSlotWidget::UpdateSlotItemImage(AC_EquipableItem* SlotItem)
{
	if (SlotItem)
	{
		ItemImage->SetBrushFromTexture(SlotItem->GetItemDatas().ItemSlotImage);
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
	//TODO : Vest�� Helmet�� ������ ǥ�ð� �ʿ���. 2������ ǥ�ð� ����. ���ڿ� ����bar, �̰� ����Ŭ�������� �ϱ�.
}
