// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUserInterface/C_MainGunWidget.h"
#include "InvenUserInterface/C_DragDropOperation.h"
//#include "Item/C_Item.h"
//#include "Blueprint/UserWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

#include "Item/Weapon/Gun/C_Gun.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_InvenSystem.h"

#include "Character/C_Player.h"

#include "Utility/C_Util.h"

#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/Gun/C_Gun.h"

void UC_MainGunWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UC_MainGunWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	//dragdrop class�� ���� ����� ����ؾ� �� �� ����.
	UC_DragDropOperation* DragOperation = NewObject<UC_DragDropOperation>();
	//if (!CachedItem) return;
	//UObject* ResourceObject = Cast<UImage>(CachedItem->GetItemDatas().ItemIcon)->Brush.GetResourceObject();//UTexture2D�ε� �Ʒ��ٿ��� �ٷ� ��� ������ �� ������?
	UTexture2D* Texture = nullptr;

	if (CachedItem->GetItemDatas().ItemType == EItemTypes::MAINGUN)
		Texture = Cast<UTexture2D>(Cast<AC_Gun>(CachedItem)->GetDragIcon());
	else
		Texture = Cast<UTexture2D>(CachedItem->GetItemDatas().ItemIcon);//ũ��� ���� �����ϱ�.

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

	DragOperation->Payload = CachedItem; // �巡�� �� ������ ������ (������)
	//DragOperation->Pivot = EDragPivot::MouseDown;
	DragOperation->Pivot = EDragPivot::CenterCenter;

	DragOperation->DraggedItem = CachedItem;

	DragOperation->curWeaponSlot = WeaponType;


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

FReply UC_MainGunWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// ��Ŭ������ üũ
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		if (CachedItem)
		{   // ��Ŭ�� �̺�Ʈ ����
			if (CachedItem->MoveToAround(OwnerCharacter))
			{
				OwnerCharacter->GetEquippedComponent()->SetSlotWeapon(EWeaponSlot::MAIN_GUN, nullptr);
				CachedItem = nullptr;
			}



			//SetVisibility(ESlateVisibility::Hidden);

			if (UC_InvenUiWidget* InvenUiWidget = GetTypedOuter<UC_InvenUiWidget>())
				InvenUiWidget->InitListView();
			Init();
			return FReply::Handled();
		}
	}
	else
	{
		UC_Util::Print("No cached item to interact with!", FColor::Red, 5.0f);
	}
	// �ٸ� ��ư Ŭ�� ó��
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UC_MainGunWidget::Init()
{
	if (IsValid(OwnerCharacter))
	{
		//Weapon = OwnerCharacter->GetEquippedComponent()->GetWeapons()[WeaponType];
		CachedItem = Cast<AC_Weapon>(OwnerCharacter->GetEquippedComponent()->GetWeapons()[WeaponType]);

	}

	if (IsValid(CachedItem))
	{
		GunImage->SetBrushFromTexture(CachedItem->GetItemDatas().ItemIcon);
		GunName->SetText(FText::FromString(CachedItem->GetItemDatas().ItemName));
		SetVisibility(ESlateVisibility::Visible);
		FSlateBrush Brush = GunImage->GetBrush();
		Brush.TintColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f); // ���� ������
		GunImage->SetBrush(Brush);
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}

void UC_MainGunWidget::SetWeapon(AC_Item* item)
{
	CachedItem = Cast<AC_Weapon>(item);
}

void UC_MainGunWidget::SetWeaponBoxNum(uint8 Num)
{
	if (!IsValid(WeaponNum)) return;
	WeaponNum->SetText(FText::FromString(FString::FromInt(Num)));

	switch (Num)
	{
	case 1:
		WeaponType = EWeaponSlot::MAIN_GUN;
		break;
	case 2:
		WeaponType = EWeaponSlot::SUB_GUN;
		break;
	default:
		break;
	}

	WeaponNum->SetVisibility(ESlateVisibility::Visible);

	Init();
}
