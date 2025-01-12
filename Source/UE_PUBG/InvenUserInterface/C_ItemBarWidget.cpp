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
	// ������ WidgetTree���� ��������
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

	// ���ε��� �������� null���� Ȯ��
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
	// ListItemObject�� UC_Item Ŭ������ ĳ�����Ͽ� ������ ������ ���
	CachedItem = Cast<AC_Item>(ListItemObject);

	if (!CachedItem) return;

	InitBar(CachedItem);
}

FReply UC_ItemBarWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// ��Ŭ������ üũ

	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		if (CachedItem)
		{   // ��Ŭ�� �̺�Ʈ ����
			CachedItem->Interaction(OwnerCharacter);
			
			InitInvenUIWidget();
			
			//NativeOnListItemObjectSet������ ȣ��� �ߺ����� �ϴ� �ּ�ó��, �ٸ� �̺�Ʈ�ÿ� �ʱ�ȭ�� �ʿ��ϸ� ����ؾ� �� �� ����.
			//if (!CachedItem) return;
			
			InitBar(CachedItem);

			//if (CachedItem->)
			//SetVisibility(ESlateVisibility::Visible);
			//AC_PlayerController* PlayerController = Cast<AC_PlayerController>(GetWorld()->GetFirstPlayerController());
			//PlayerController->SetIgnoreMoveInput(false); // �̵� ���
			//return FReply::Unhandled();
		

			return FReply::Handled();

		}
	}
	else
	{
		UC_Util::Print("No cached item to interact with!", FColor::Red, 5.0f);
	}
	// �ٸ� ��ư Ŭ�� ó��
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	//return FReply::Unhandled();
}

//FReply UC_ItemBarWidget::NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
//{
//	// Tab Ű�� ���ȴ��� Ȯ��
//	if (InKeyEvent.GetKey() == EKeys::Tab)
//	{
//		// Tab Ű �Է��� ó���ϰ� �� �̻� ���ĵ��� �ʵ��� ��
//		//OwnerCharacter->GetInvenSystem()->OpenInvenUI();
//		return FReply::Handled();
//
//	}
//
//	// �ٸ� Ű �Է��� �⺻ ó���� �Ѿ
//	return Super::NativeOnKeyDown(MyGeometry, InKeyEvent);
//}

void UC_ItemBarWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	AC_PlayerController* PlayerController = Cast<AC_PlayerController>(GetWorld()->GetFirstPlayerController());
	PlayerController->SetIgnoreMoveInput(false); // �̵� ���
	//dragdrop class�� ���� ����� ����ؾ� �� �� ����.
	UC_DragDropOperation* DragOperation = NewObject<UC_DragDropOperation>();
	
	UTexture2D* Texture = Cast<UTexture2D>(CachedItem->GetItemDatas().ItemBarIcon);//ũ��� ���� �����ϱ�.

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
	//Border->SetUserFocus(Cast<AC_PlayerController>(OwnerCharacter->GetController()));

	//UObject* ResourceObject = ItemImage1->Brush.GetResourceObject();
	//UTexture2D* Texture = Cast<UTexture2D>(ResourceObject);
	//
	//UImage* DragVisual = NewObject<UImage>(Texture);
	//DragVisual->SetBrushFromTexture(Texture);
	//
	//DragOperation->DefaultDragVisual = DragVisual;

	//DragOperation->DefaultDragVisual = ItemImage1; // �巡�� �� �������� �̸����� �̹���
	//DragOperation->DefaultDragVisual = this; // �巡�� �� �������� �̸����� �̹���

	DragOperation->Payload = CachedItem; // �巡�� �� ������ ������ (������)
	//DragOperation->Pivot = EDragPivot::MouseDown;
	DragOperation->Pivot = EDragPivot::CenterCenter;

	DragOperation->DraggedItem = CachedItem;


	//����ĳ���� üũ
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

FReply UC_ItemBarWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//AC_PlayerController* PlayerController = Cast<AC_PlayerController>(GetWorld()->GetFirstPlayerController());
	//PlayerController->SetIgnoreMoveInput(false); // �̵� ���
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		//AC_PlayerController* PlayerController = Cast<AC_PlayerController>(GetWorld()->GetFirstPlayerController());
		//PlayerController->SetIgnoreMoveInput(false); // �̵� ���
		if (CachedItem)
		{
			//�巡�� �̺�Ʈ ����.
			//OwnerCharacter->GetController()->SetIgnoreMoveInput(false);
			// �巡�׸� �����ϰ� ������
			FEventReply RePlyResult =
				UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
			UC_Util::Print("LeftMouseButton");
			OwnerCharacter->GetInvenSystem()->GetInvenUI()->SetIsDragging(true);

			//AC_PlayerController* PlayerController = Cast<AC_PlayerController>(GetWorld()->GetFirstPlayerController());
			//PlayerController->SetIgnoreMoveInput(false); // �̵� ���
			//PlayerController->GetCurrentInputModeDebugString();

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

		ItemImage1->SetBrushFromTexture(item->GetItemDatas().ItemBarIcon);

		ItemType = item->GetItemDatas().ItemType;

		ItemName1->SetText(FText::FromString(item->GetItemDatas().ItemName));


		if (item->GetItemDatas().ItemCurStack == 0)
			ItemStackBlock1->SetVisibility(ESlateVisibility::Hidden);
		else
			ItemStackBlock1->SetText(FText::AsNumber(item->GetItemDatas().ItemCurStack));
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
			CachedItem->GetItemDatas().ItemType == EItemTypes::CONSUMPTIONITEM
			|| CachedItem->GetItemDatas().ItemType == EItemTypes::THROWABLE
			|| CachedItem->GetItemDatas().ItemType == EItemTypes::MELEEWEAPON
			|| CachedItem->GetItemDatas().ItemType == EItemTypes::ATTACHMENT
			)
		{
			InvenUiWidget->InitListView();
		}

		InvenUiWidget->InitWidget();
	}
}

