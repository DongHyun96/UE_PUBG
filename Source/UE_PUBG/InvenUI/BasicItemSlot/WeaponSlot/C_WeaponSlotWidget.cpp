// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/BasicItemSlot/WeaponSlot/C_WeaponSlotWidget.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenSystem.h"

#include "Components/CanvasPanel.h"

#include "InvenUserInterface/C_DragDropOperation.h"


FReply UC_WeaponSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 우클릭인지 체크
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		AC_Weapon* SlotItem = OwnerPlayer->GetEquippedComponent()->GetWeapons()[WeaponType];

		if (SlotItem)
		{  
			// 우클릭 이벤트 실행
			// slot에서는 장착아이템이 존재하면 해제하는 함수를 실행한다.
			MouseRBDownInteraction(SlotItem);
			//{
			//	OwnerPlayer->GetEquippedComponent()->SetSlotWeapon(WeaponType, nullptr);
			//}

			if (UC_InventoryUIWidget* InvenUiWidget = GetTypedOuter<UC_InventoryUIWidget>())
				InvenUiWidget->UpdateWidget();
			return FReply::Handled();
		}
	}
	// 다른 버튼 클릭 처리
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UC_WeaponSlotWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		AC_Weapon* SlotItem = OwnerPlayer->GetEquippedComponent()->GetWeapons()[WeaponType];

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

void UC_WeaponSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	//dragdrop class를 새로 만들어 사용해야 할 수 있음.
	UC_DragDropOperation* DragOperation = NewObject<UC_DragDropOperation>();
	//if (!CachedItem) return;
	//UObject* ResourceObject = Cast<UImage>(CachedItem->GetItemDatas().ItemIcon)->Brush.GetResourceObject();//UTexture2D인데 아랫줄에서 바로 사용 가능할 것 같은데?
	UTexture2D* Texture = nullptr;

	AC_Item* SlotItem = OwnerPlayer->GetEquippedComponent()->GetWeapons()[WeaponType];

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
	//TODO : Gun의 경우 사용하는 총알의 종류와 갯수를 표시해줌. 하위 클래스에서 진행할 예정.

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
		WeaponSlotPanel->SetVisibility(ESlateVisibility::Hidden); //TODO : Hidden과 Collaps? 이거 차이 생각하기.
	}
}

bool UC_WeaponSlotWidget::MouseRBDownInteraction(AC_Weapon* inSlotWeapon)
{
	return false;
}
