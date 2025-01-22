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
		{   // 우클릭 이벤트 실행
			//EquippedItem->Interaction(OwnerCharacter);
			SlotItem->MoveToAround(OwnerPlayer);
			//InitInvenUIWidget();

			//NativeOnListItemObjectSet에서의 호출과 중복으로 일단 주석처리, 다만 이벤트시에 초기화가 필요하면 사용해야 할 수 있음.
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
	// 다른 버튼 클릭 처리
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UC_EquipmentSlotWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		AC_EquipableItem* SlotItem = OwnerPlayer->GetInvenComponent()->GetEquipmentItems()[EquipSlot];

		if (SlotItem)
		{
			//드래그 이벤트 실행.
			//드래그를 시작하고 반응함
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
	//dragdrop class를 새로 만들어 사용해야 할 수 있음.
	UC_DragDropOperation* DragOperation = NewObject<UC_DragDropOperation>();
	//if (!CachedItem) return;
	//UObject* ResourceObject = Cast<UImage>(CachedItem->GetItemDatas().ItemIcon)->Brush.GetResourceObject();//UTexture2D인데 아랫줄에서 바로 사용 가능할 것 같은데?
	UTexture2D* Texture = nullptr;

	AC_EquipableItem* SlotItem = OwnerPlayer->GetInvenComponent()->GetEquipmentItems()[EquipSlot];


	if (!SlotItem) return;

	Texture = Cast<UTexture2D>(SlotItem->GetItemDatas().ItemBarIcon);

	UBorder* Border = NewObject<UBorder>();
	FLinearColor BorderColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.1f); // (R, G, B, A)
	Border->SetBrushColor(BorderColor);

	//Border->SetPadding(FMargin(2.0f)); 패딩이 필요하면 사용하기.

	UImage* DragVisual = NewObject<UImage>(Texture);

	DragVisual->SetBrushFromTexture(Texture);
	//DragVisual->SetBrushFromTexture(Texture);
	DragVisual->Brush.ImageSize = FVector2D(64.f, 64.f);
	Border->SetContent(DragVisual);

	DragOperation->DefaultDragVisual = Border;

	//DragOperation->DefaultDragVisual = ItemImage1; // 드래그 시 아이템의 미리보기 이미지
	//DragOperation->DefaultDragVisual = this; // 드래그 시 아이템의 미리보기 이미지

	DragOperation->Payload = SlotItem; // 드래그 중 전달할 데이터 (아이템)
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
	//TODO : Vest와 Helmet은 내구도 표시가 필요함. 2가지의 표시가 있음. 숫자와 붉은bar, 이건 하위클래스에서 하기.
}

void UC_EquipmentSlotWidget::UpdateSlotItemImage(AC_EquipableItem* SlotItem)
{
	if (SlotItem)
	{
		ItemImage->SetBrushFromTexture(SlotItem->GetItemDatas().ItemSlotImage);
		FSlateBrush Brush = ItemImage->GetBrush();
		Brush.TintColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
		ItemImage->SetBrush(Brush);
		//SetVisibility(ESlateVisibility::Visible); //TODO : 필요한 기능인가? 적절한 기능인가? 체크하고 결정하기.
	}
	else
	{
		ItemImage->SetBrushFromTexture(nullptr);
		FSlateBrush Brush = ItemImage->GetBrush();
		Brush.TintColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.3f);
		ItemImage->SetBrush(Brush);
	}
	//TODO : Vest와 Helmet은 내구도 표시가 필요함. 2가지의 표시가 있음. 숫자와 붉은bar, 이건 하위클래스에서 하기.
}
