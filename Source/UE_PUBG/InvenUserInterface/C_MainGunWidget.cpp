// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUserInterface/C_MainGunWidget.h"
//#include "Item/C_Item.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"

#include "Utility/C_Util.h"

#include "Item/Weapon/C_Weapon.h"

void UC_MainGunWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

FReply UC_MainGunWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 우클릭인지 체크
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		if (Weapon)
		{   // 우클릭 이벤트 실행
			if (Weapon->MoveToAround(OwnerCharacter))
			{
				OwnerCharacter->GetEquippedComponent()->SetSlotWeapon(EWeaponSlot::MAIN_GUN, nullptr);
				Weapon = nullptr;
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
	// 다른 버튼 클릭 처리
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UC_MainGunWidget::Init()
{
	if (IsValid(OwnerCharacter))
	{
		Weapon = OwnerCharacter->GetEquippedComponent()->GetWeapons()[WeaponType];
	}

	if (IsValid(Weapon))
	{
		GunImage->SetBrushFromTexture(Weapon->GetItemDatas().ItemIcon);
		GunName->SetText(FText::FromString(Weapon->GetItemDatas().ItemName));
		SetVisibility(ESlateVisibility::Visible);
		FSlateBrush Brush = GunImage->GetBrush();
		Brush.TintColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f); // 완전 불투명
		GunImage->SetBrush(Brush);
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}

void UC_MainGunWidget::SetWeapon(AC_Item* item)
{
	Weapon = Cast<AC_Weapon>(item);
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
