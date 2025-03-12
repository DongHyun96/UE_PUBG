// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUserInterface/C_ItemBarWidget.h"
#include "InvenUserInterface/C_InvenUiWidget.h"
#include "InvenUserInterface/C_DragDropOperation.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "Item/C_Item.h"
#include "TimerManager.h"

#include "Character/Component/C_InvenSystem.h"
#include "Character/Component/C_PlayerController.h"
//#include "NavigationSystem.h"
#include "Utility/C_Util.h"

void UC_ItemBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 위젯을 WidgetTree에서 가져오기
	//ItemImage = Cast<UImage>(GetWidgetFromName(FName("ItemImage1")));
	//ItemName = Cast<UTextBlock>(GetWidgetFromName(FName("ItemName1")));
	//ItemStackBlock = Cast<UTextBlock>(GetWidgetFromName(FName("ItemStackBlock1")));

	//ItemImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), FName("ItemImage1"));
	//ItemName = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName("ItemName1"));
	//ItemStackBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName("ItemStackBlock1"));
	//this->SetIsFocusable(true);

	//SetIsFocusable(false);

	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<AC_Player>(GetOwningPlayerPawn());
	}

	if (!CachedItem) return;

	InitBar(CachedItem);

	// 바인딩된 위젯들이 null인지 확인
	if (!ItemImage1)
	{
		UC_Util::Print("ItemImage is not bound correctly!", FColor::Red, 10.f);
	}
	if (!ItemName1)
	{
		UC_Util::Print("ItemName is not bound correctly!", FColor::Red, 10.f);
	}
	if (!ItemStackBlock1)
	{
		UC_Util::Print("ItemStackBlock is not bound correctly!", FColor::Red, 10.f);
	}
}

void UC_ItemBarWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	// ListItemObject를 UC_Item 클래스로 캐스팅하여 아이템 데이터 사용
	CachedItem = Cast<AC_Item>(ListItemObject);

	if (!CachedItem) return;

	InitBar(CachedItem);
}

FReply UC_ItemBarWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 우클릭인지 체크

	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		if (CachedItem)
		{   // 우클릭 이벤트 실행
			CachedItem->Interaction(OwnerCharacter);
			
			InitInvenUIWidget();
			
			//NativeOnListItemObjectSet에서의 호출과 중복으로 일단 주석처리, 다만 이벤트시에 초기화가 필요하면 사용해야 할 수 있음.
			//if (!CachedItem) return;
			
			InitBar(CachedItem);

			//if (CachedItem->)
			//SetVisibility(ESlateVisibility::Visible);
			//AC_PlayerController* PlayerController = Cast<AC_PlayerController>(GetWorld()->GetFirstPlayerController());
			//PlayerController->SetIgnoreMoveInput(false); // 이동 허용
			//return FReply::Unhandled();

			return FReply::Handled();
			//return FReply::Unhandled();

		}
	}
	else
	{
		UC_Util::Print("No cached item to interact with!", FColor::Red, 5.0f);
	}
	// 다른 버튼 클릭 처리
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	//return FReply::Unhandled();
}

//FReply UC_ItemBarWidget::NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
//{
//	// Tab 키가 눌렸는지 확인
//	if (InKeyEvent.GetKey() == EKeys::Tab)
//	{
//		// Tab 키 입력을 처리하고 더 이상 전파되지 않도록 함
//		//OwnerCharacter->GetInvenSystem()->OpenInvenUI();
//		return FReply::Handled();
//
//	}
//
//	// 다른 키 입력은 기본 처리로 넘어감
//	return FReply::Unhandled(); //Super::NativeOnKeyDown(MyGeometry, InKeyEvent);
//}

void UC_ItemBarWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	AC_PlayerController* PlayerController = Cast<AC_PlayerController>(GetWorld()->GetFirstPlayerController());
	PlayerController->SetIgnoreMoveInput(false); // 이동 허용
	//dragdrop class를 새로 만들어 사용해야 할 수 있음.
	UC_DragDropOperation* DragOperation = NewObject<UC_DragDropOperation>();
	
	UTexture2D* Texture = Cast<UTexture2D>(CachedItem->GetItemDatas()->ItemBarIcon);//크기및 형태 조절하기.

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
	//Border->SetUserFocus(Cast<AC_PlayerController>(OwnerCharacter->GetController()));

	//UObject* ResourceObject = ItemImage1->Brush.GetResourceObject();
	//UTexture2D* Texture = Cast<UTexture2D>(ResourceObject);
	//
	//UImage* DragVisual = NewObject<UImage>(Texture);
	//DragVisual->SetBrushFromTexture(Texture);
	//
	//DragOperation->DefaultDragVisual = DragVisual;

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

	PlayerController->SetInputMode
	(
		FInputModeGameAndUI()
		.SetWidgetToFocus(nullptr)
		.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock)
		.SetHideCursorDuringCapture(false)
	);
	PlayerController->bShowMouseCursor = true;
	PlayerController->SetIgnoreLookInput(true);
}

FReply UC_ItemBarWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	AC_PlayerController* PlayerController = Cast<AC_PlayerController>(GetWorld()->GetFirstPlayerController());
	//PlayerController->SetIgnoreMoveInput(false); // 이동 허용
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		//AC_PlayerController* PlayerController = Cast<AC_PlayerController>(GetWorld()->GetFirstPlayerController());
		//PlayerController->SetIgnoreMoveInput(false); // 이동 허용
		if (CachedItem)
		{
			//드래그 이벤트 실행.
			//OwnerCharacter->GetController()->SetIgnoreMoveInput(false);
			// 드래그를 시작하고 반응함
			FEventReply RePlyResult =
				UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
			UC_Util::Print("LeftMouseButton");
			OwnerCharacter->GetInvenSystem()->GetInvenUI()->SetIsDragging(true);

			//AC_PlayerController* PlayerController = Cast<AC_PlayerController>(GetWorld()->GetFirstPlayerController());
			//PlayerController->SetIgnoreMoveInput(false); // 이동 허용
			//PlayerController->GetCurrentInputModeDebugString();
			PlayerController->SetInputMode
			(
				FInputModeGameAndUI()
				.SetWidgetToFocus(nullptr)
				.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock)
				.SetHideCursorDuringCapture(false)
			);
			PlayerController->bShowMouseCursor = true;
			PlayerController->SetIgnoreLookInput(true);

			return RePlyResult.NativeReply;

			//return FReply::Unhandled();
		}
	}
	return Super::NativeOnPreviewMouseButtonDown(InGeometry,InMouseEvent);
	
	//return FReply::Unhandled();
}

void UC_ItemBarWidget::InitBar(AC_Item* item)
{


	//ItemImage = Cast<UImage>(GetWidgetFromName(FName("ItemImage1")));
	//ItemName = Cast<UTextBlock>(GetWidgetFromName(FName("ItemName1")));
	//ItemStackBlock = Cast<UTextBlock>(GetWidgetFromName(FName("ItemStackBlock1")));
	//SetIsFocusable(false);

	//bIsFocusable
	if (item)
	{
		CachedItem = item;

		const FItemData* CachedItemData = CachedItem->GetItemDatas();

		ItemImage1->SetBrushFromTexture(CachedItemData->ItemBarIcon);

		ItemType = CachedItemData->ItemType;

		ItemName1->SetText(FText::FromString(CachedItemData->ItemName));


		if (item->GetItemCurStack() == 0)
			ItemStackBlock1->SetVisibility(ESlateVisibility::Hidden);
		else
			ItemStackBlock1->SetText(FText::AsNumber(item->GetItemCurStack()));
		//AddToViewport();
		//SetVisibility(ESlateVisibility::Visible);

	}
	else
	{
		UC_Util::Print("NO CachedItem!!", FColor::Red, 5.0f);
		//SetVisibility(ESlateVisibility::Hidden);
		//RemoveFromViewport();
		UC_Util::Print("Visibility::Hidden");
	}

	//FInputModeGameAndUI InputMode;
	//InputMode.SetWidgetToFocus(nullptr);
	//InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	//InputMode.SetHideCursorDuringCapture(true);
	//
	//APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	//PlayerController->SetInputMode(InputMode);
}

AC_Item* UC_ItemBarWidget::GetItem(AC_Item* nearItem)
{
	return nullptr;
}

AC_Item* UC_ItemBarWidget::DropItem(AC_Item* myItem)
{
	return nullptr;
}

void UC_ItemBarWidget::SetPercent(float curTime, float endTime)
{
	ItemUsingTimer->SetPercent(curTime / endTime);
}

void UC_ItemBarWidget::InitInvenUIWidget()
{
	if (UC_InvenUiWidget* InvenUiWidget = GetTypedOuter<UC_InvenUiWidget>())
	{
		if (
			   CachedItem->GetItemDatas()->ItemType == EItemTypes::CONSUMPTIONITEM
			|| CachedItem->GetItemDatas()->ItemType == EItemTypes::THROWABLE
			|| CachedItem->GetItemDatas()->ItemType == EItemTypes::MELEEWEAPON
			|| CachedItem->GetItemDatas()->ItemType == EItemTypes::ATTACHMENT
			)
		{
			InvenUiWidget->InitListView();
		}

		InvenUiWidget->InitWidget();
	}
}



