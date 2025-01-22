// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/ItemBar/C_BasicItemBarWidget.h"
#include "InvenUserInterface/C_InvenUiWidget.h"
#include "InvenUserInterface/C_DragDropOperation.h"

#include "Character/C_Player.h"

#include "Character/Component/C_PlayerController.h"
#include "Character/Component/C_InvenSystem.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Progressbar.h"

#include "Kismet/GameplayStatics.h"

//#include "Blueprint/WidgetBlueprintLibrary.h"

void UC_BasicItemBarWidget::NativeConstruct()
{
	bIsFocusable = false;
}

FReply UC_BasicItemBarWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		//��Ŭ�� �̺�Ʈ ����
		//TODO : ����
		if (!CachedItem) return FReply::Handled();

		AC_Player* OwnerPlayer = Cast<AC_Player>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));//ItemBar�� ������ OwnerPlayer�� ������ �޸� ���� ���Ұ� ���Ƽ� ���
		
		CachedItem->Interaction(OwnerPlayer);

		UpdateWidget(CachedItem);

		UpdateInvenUIWidget();

		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UC_BasicItemBarWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		if (CachedItem)
		{
			//�巡�� �̺�Ʈ ����.
			//�巡�׸� �����ϰ� ������
			FEventReply RePlyResult =
				UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
			AC_Player* OwnerPlayer = Cast<AC_Player>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
			//UC_Util::Print("LeftMouseButton");
			OwnerPlayer->GetInvenSystem()->GetInvenUI()->SetIsDragging(true);

			return RePlyResult.NativeReply;
		}
	}
	return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);

}

void UC_BasicItemBarWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	AC_Player* OwnerPlayer = Cast<AC_Player>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

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

	DragOperation->curWeaponSlot = EWeaponSlot::NONE;

	//����ĳ���� üũ
	if (!OwnerPlayer)
	{
		//UC_Util::Print("ItemBarWidget have not OwnerCharacter!!");
		return;
	}
	this->Visibility = ESlateVisibility::SelfHitTestInvisible;
	OwnerPlayer->GetInvenSystem()->GetInvenUI()->SetIsDragging(true);

	//OwnerCharacter->GetInvenSystem()->GetInvenUI()->SetItemListZorder(CachedItem->GetOwnerCharacter());


	//UC_Util::Print("OnDragDetected!!");

	OutOperation = DragOperation;
}

void UC_BasicItemBarWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	// ListItemObject�� UC_Item Ŭ������ ĳ�����Ͽ� ������ ������ ���
	CachedItem = Cast<AC_Item>(ListItemObject);

	if (!CachedItem) return;

	UpdateWidget(CachedItem);
}

void UC_BasicItemBarWidget::UpdateWidget(AC_Item* MyItem)
{
	if (MyItem)
	{
		CachedItem = MyItem;

		ItemImage->SetBrushFromTexture(MyItem->GetItemDatas().ItemBarIcon);

		ItemType = MyItem->GetItemDatas().ItemType;

		ItemName->SetText(FText::FromString(MyItem->GetItemDatas().ItemName));
		SetVisibility(ESlateVisibility::Visible);

		


		if (MyItem->GetItemDatas().ItemCurStack == 0)
		{
			//TODO : ��������� �������� ������ �������� curStack�� 0�� �Ǹ� ��������� Inven���� �����ϱ�.
			if (MyItem->GetItemDatas().ItemType == EItemTypes::MELEEWEAPON && MyItem->GetItemDatas().ItemType == EItemTypes::ATTACHMENT)
			{
				ItemStackBlock->SetVisibility(ESlateVisibility::Hidden);
			}
			else
			{
				CachedItem = nullptr;

				//SetVisibility(ESlateVisibility::Hidden);
			}
		}
		else
			ItemStackBlock->SetText(FText::AsNumber(MyItem->GetItemDatas().ItemCurStack));
		//AddToViewport();
		//SetVisibility(ESlateVisibility::Visible);

	}
	//UpdateInvenUIWidget();

}

void UC_BasicItemBarWidget::SetPercent(float curTime, float endTime)
{
	ItemUsingTimer->SetPercent(curTime / endTime);
}

void UC_BasicItemBarWidget::UpdateInvenUIWidget()
{
	if (UC_InventoryUIWidget* InvenUiWidget = GetTypedOuter<UC_InventoryUIWidget>())
	{
		if (
			CachedItem->GetItemDatas().ItemType == EItemTypes::CONSUMPTIONITEM
			|| CachedItem->GetItemDatas().ItemType == EItemTypes::THROWABLE
			|| CachedItem->GetItemDatas().ItemType == EItemTypes::MELEEWEAPON
			|| CachedItem->GetItemDatas().ItemType == EItemTypes::ATTACHMENT
			)
		{
			InvenUiWidget->InitializeListView();
		}

		InvenUiWidget->UpdateWidget();
	}
}
