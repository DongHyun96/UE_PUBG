// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUserInterface/C_BasicSlotWidget.h"
#include "InvenUserInterface/C_DragDropOperation.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "Character/C_Player.h"
#include "Character/Component/C_InvenSystem.h"

#include "Components/Image.h"
#include "Components/Border.h"

#include "Utility/C_Util.h"


void UC_BasicSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	//dragdrop class를 새로 만들어 사용해야 할 수 있음.
	UC_DragDropOperation* DragOperation = NewObject<UC_DragDropOperation>();
	//if (!CachedItem) return;
	//UObject* ResourceObject = Cast<UImage>(CachedItem->GetItemDatas().ItemIcon)->Brush.GetResourceObject();//UTexture2D인데 아랫줄에서 바로 사용 가능할 것 같은데?
	UTexture2D* Texture = Cast<UTexture2D>(CachedItem->GetItemDatas().ItemIcon);//크기및 형태 조절하기.

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

	DragOperation->Payload = CachedItem; // 드래그 중 전달할 데이터 (아이템)
	//DragOperation->Pivot = EDragPivot::MouseDown;
	DragOperation->Pivot = EDragPivot::CenterCenter;

	DragOperation->DraggedItem = CachedItem;


	//오너캐릭터 체크
	if (!OwnerCharacter)
	{
		UC_Util::Print("ItemBarWidget have not OwnerCharacter!!");
		return;
	}
	this->Visibility = ESlateVisibility::SelfHitTestInvisible;
	OwnerCharacter->GetInvenSystem()->GetInvenUI()->SetIsDragging(true);

	//OwnerCharacter->GetInvenSystem()->GetInvenUI()->SetItemListZorder(CachedItem->GetOwnerCharacter());


	UC_Util::Print("OnDragDetected!!");

	OutOperation = DragOperation;
}

FReply UC_BasicSlotWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		if (CachedItem)
		{
			//드래그 이벤트 실행.
			if (!IsValid(OwnerCharacter)) return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
			// 드래그를 시작하고 반응함
			FEventReply RePlyResult =
				UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
			OwnerCharacter->GetInvenSystem()->GetInvenUI()->SetIsDragging(true);

			return RePlyResult.NativeReply;

			//return FReply::Handled();
		}
	}
	return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
}

void UC_BasicSlotWidget::HandleDragWidgetStart()
{
}


