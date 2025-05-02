// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/EquipmentSlot/C_AttachableItemSlotWidget.h"
#include "InvenUI/C_InventoryUIWidget.h"

#include "Character/C_Player.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenSystem.h"

#include "Components/Image.h"
#include "Components/Border.h"

//#include "InvenUserInterface/C_DragDropOperation.h"
#include "C_DragDropOperation.h"

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
			//드래그 이벤트 실행.
			//드래그를 시작하고 반응함
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
		{   // 우클릭 이벤트 실행
			//EquippedItem->Interaction(OwnerCharacter);
			SlotItem->MoveToInven(OwnerPlayer, SlotItem->GetItemCurStack());
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
	// 다른 버튼 클릭 처리
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

void UC_AttachableItemSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	//dragdrop class를 새로 만들어 사용해야 할 수 있음.
	UC_DragDropOperation* DragOperation = NewObject<UC_DragDropOperation>();
	//if (!CachedItem) return;
	//UObject* ResourceObject = Cast<UImage>(CachedItem->GetItemDatas().ItemIcon)->Brush.GetResourceObject();//UTexture2D인데 아랫줄에서 바로 사용 가능할 것 같은데?
	UTexture2D* Texture = nullptr;

	AC_Gun* curGun = Cast<AC_Gun>(OwnerPlayer->GetEquippedComponent()->GetWeapons()[GunSlot]);
	
	if (!curGun) return;

	AC_AttachableItem* SlotItem = curGun->GetAttachableItem()[SlotName];

	if (!SlotItem) return;

	Texture = Cast<UTexture2D>(SlotItem->GetItemDatas()->ItemBarIcon);

	UBorder* Border = NewObject<UBorder>();
	FLinearColor BorderColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.1f); // (R, G, B, A)
	Border->SetBrushColor(BorderColor);

	UImage* DragVisual = NewObject<UImage>(Texture);

	DragVisual->SetBrushFromTexture(Texture);
	DragVisual->Brush.ImageSize = FVector2D(64.f, 64.f);
	Border->SetContent(DragVisual);

	DragOperation->DefaultDragVisual = Border;
	DragOperation->Payload = SlotItem; // 드래그 중 전달할 데이터 (아이템)
	DragOperation->Pivot = EDragPivot::MouseDown;

	FVector2D MousePosition = InMouseEvent.GetScreenSpacePosition();
	// 현재 마우스 클릭 위치 가져오기 (화면 좌표)
	FVector2D Offset = DragVisual->Brush.ImageSize * 0.5f;
	FVector2D CenteredPosition = MousePosition - Offset;

	// 현재 위젯(ItemBar)의 화면 좌표 가져오기
	FVector2D WidgetScreenPosition = InGeometry.AbsoluteToLocal(CenteredPosition); //왜 이걸 써야만 하는가?

	// 드래그 비주얼 위치를 강제로 설정 (렌더링 기준으로 설정)
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
		ItemImage->SetBrushFromTexture(curItem->GetItemDatas()->ItemSlotImage);
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
}

void UC_AttachableItemSlotWidget::DiscriminateSlot(TArray<EPartsName> AttachableSlots, AC_Gun* SlotItem)
{
	for (EPartsName Name : AttachableSlots)
	{
		if (SlotName == Name)
		{
			this->UpdateSlotItemImage(SlotItem);
			this->SetVisibility(ESlateVisibility::Visible); //Widget 활성화.
			return;
		}
	}
	this->SetVisibility(ESlateVisibility::Collapsed); // Widget 비활성화.
}

//void UC_AttachableItemSlotWidget::DiscriminateSlot(TArray<EPartsName> AttachableSlots, AC_Gun SlotItem)
//{
//	for (EPartsName Name : AttachableSlots)
//	{
//		if (SlotName == Name)
//		{
//			this->UpdateSlotItemImage(SlotItem);
//			this->SetVisibility(ESlateVisibility::Visible); //Widget 활성화.
//			return;
//		}
//	}
//	this->SetVisibility(ESlateVisibility::Collapsed); // Widget 비활성화.
//}


