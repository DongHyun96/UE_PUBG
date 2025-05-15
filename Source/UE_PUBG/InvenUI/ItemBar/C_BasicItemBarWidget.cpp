// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/ItemBar/C_BasicItemBarWidget.h"
#include "InvenUI/Panel/C_BasicPanelWidget.h"
#include "InvenUserInterface/C_DragDropOperation.h"
//#include "C_DragDropOperation.h"

#include "Character/C_Player.h"

#include "Character/Component/C_PlayerController.h"
#include "Character/Component/C_InvenSystem.h"
#include "Character/Component/C_InvenComponent.h"

#include "Item/ConsumableItem/C_ConsumableItem.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Progressbar.h"
#include "Singleton/C_GameSceneManager.h"
#include "Kismet/GameplayStatics.h"

//#include "Blueprint/WidgetBlueprintLibrary.h"

void UC_BasicItemBarWidget::NativeConstruct()
{
	bIsFocusable = false;
}

FReply UC_BasicItemBarWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton) && InMouseEvent.IsLeftAltDown())
	{
		if (HalfStackItemInteraction()) // true면 return, false면 남은 코드 실행.
		{
			UpdateInvenUIWidget();
			return FReply::Handled(); 
		}
	}

	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		//우클릭 이벤트 실행
		//TODO : 구현
		if (!CachedItem) return FReply::Handled();

		if (InMouseEvent.IsAltDown())
			if (HalfStackItemInteraction()) return FReply::Handled(); //참이면 return, 거짓이면 남은 코드 실행.

		AC_Player* OwnerPlayer = GAMESCENE_MANAGER->GetPlayer();
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
			//드래그 이벤트 실행.
			//드래그를 시작하고 반응함
			FEventReply RePlyResult =
				UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
			//AC_Player* OwnerPlayer = Cast<AC_Player>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)); //TODO : 게임 신 매니저에서 플레이어 가져오기
			AC_Player* OwnerPlayer = GAMESCENE_MANAGER->GetPlayer();
			//UC_Util::Print("LeftMouseButton");
			OwnerPlayer->GetInvenSystem()->GetInvenUI()->SetIsDragging(true);

			return RePlyResult.NativeReply;
		}
	}
	return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);

}

void UC_BasicItemBarWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	AC_Player* OwnerPlayer = GAMESCENE_MANAGER->GetPlayer();

	AC_PlayerController* PlayerController = Cast<AC_PlayerController>(GetWorld()->GetFirstPlayerController());
	PlayerController->SetIgnoreMoveInput(false); // 이동 허용
	//dragdrop class를 새로 만들어 사용해야 할 수 있음.
	UC_DragDropOperation* DragOperation = NewObject<UC_DragDropOperation>();

	UTexture2D* Texture = Cast<UTexture2D>( CachedItem->GetItemDatas()->ItemBarIcon);//크기및 형태 조절하기.

	UBorder* Border = NewObject<UBorder>();
	FLinearColor BorderColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.1f); // (R, G, B, A)
	Border->SetBrushColor(BorderColor);

	UImage* DragVisual = NewObject<UImage>(Texture);

	DragVisual->SetBrushFromTexture(Texture);
	DragVisual->Brush.ImageSize = FVector2D(64.f, 64.f);
	Border->SetContent(DragVisual);

	DragOperation->DefaultDragVisual = Border;

	DragOperation->Payload = CachedItem; // 드래그 중 전달할 데이터 (아이템)
	DragOperation->Pivot = EDragPivot::MouseDown;
	FVector2D MousePosition = InMouseEvent.GetScreenSpacePosition();

	// 현재 마우스 클릭 위치 가져오기 (화면 좌표)
	FVector2D Offset = DragVisual->Brush.ImageSize * 0.5f;
	FVector2D CenteredPosition = MousePosition - Offset;

	// 현재 위젯(ItemBar)의 화면 좌표 가져오기
	FVector2D WidgetScreenPosition = InGeometry.AbsoluteToLocal(CenteredPosition); //왜 이걸 써야만 하는가?

	// 드래그 비주얼 위치를 강제로 설정 (렌더링 기준으로 설정)
	Border->SetRenderTranslation(WidgetScreenPosition);

	//TODO : UDragDropOperation내의 DraggedItem을 UItemBox의 DraggedItemBox로 대체하기. 
	DragOperation->DraggedItem =  CachedItem;

	DragOperation->curWeaponSlot = EWeaponSlot::NONE;

	//오너캐릭터 체크
	if (!OwnerPlayer)
	{
		//UC_Util::Print("ItemBarWidget have not OwnerCharacter!!");
		return;
	}
	this->Visibility = ESlateVisibility::SelfHitTestInvisible;
	OwnerPlayer->GetInvenSystem()->GetInvenUI()->SetIsDragging(true);

	OutOperation = DragOperation;
}

void UC_BasicItemBarWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	// ListItemObject를 UC_Item 클래스로 캐스팅하여 아이템 데이터 사용
	CachedItem = Cast<AC_Item>(ListItemObject);

	if (CachedItem)
	{
		UpdateWidget(CachedItem);
		if (AC_ConsumableItem* ConsumableItem = Cast<AC_ConsumableItem>(CachedItem))
			if (!ConsumableItem->GetLinkedItemBarWidget()) ConsumableItem->SetLinkedItemBarWidget(this);

		return;
	}
}

void UC_BasicItemBarWidget::UpdateWidget(AC_Item* MyItem)
{
	if (MyItem)
	{
		CachedItem = MyItem;

		const FItemData* CachedItemData =  CachedItem->GetItemDatas();

		ItemImage->SetBrushFromTexture(CachedItemData->ItemBarIcon);

		ItemType = CachedItemData->ItemType;

		ItemName->SetText(FText::FromString(CachedItemData->ItemName));

		SetVisibility(ESlateVisibility::Visible);
	}
}

void UC_BasicItemBarWidget::SetPercent(float curTime, float endTime)
{
	ItemUsingTimer->SetPercent(curTime / endTime);
}

void UC_BasicItemBarWidget::UpdateInvenUIWidget()
{
	if (UC_InventoryUIWidget* InvenUiWidget = GetTypedOuter<UC_InventoryUIWidget>())
	{
		const FItemData* CachedItemData =  CachedItem->GetItemDatas();

		if (
			   CachedItemData->ItemType == EItemTypes::CONSUMPTIONITEM
			|| CachedItemData->ItemType == EItemTypes::THROWABLE
			|| CachedItemData->ItemType == EItemTypes::MELEEWEAPON
			|| CachedItemData->ItemType == EItemTypes::ATTACHMENT
			)
		{
			InvenUiWidget->UpdateAroundItemPanelWidget();
			InvenUiWidget->UpdateInventroyItemPanelWidget();
		}

		InvenUiWidget->UpdateWidget();
	}
}

bool UC_BasicItemBarWidget::HalfStackItemInteraction()
{
	int CachedItemCurStack =  CachedItem->GetItemCurStack();
	if (CachedItemCurStack < 2) return false; //만약 뒤의 우클릭으로 안넘어간다면 여기서 Interaction 해야함.

	AC_Player* PlayerCharacter = Cast<AC_Player>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if (!PlayerCharacter) return false; //PlayerCharacter가 없다면 return;

	uint32 HalfStack = CachedItemCurStack * 0.5;

	float RestVolume = PlayerCharacter->GetInvenComponent()->GetMaxVolume() - PlayerCharacter->GetInvenComponent()->GetCurVolume();

	if (CachedItem->GetItemPlace() == EItemPlace::AROUND)
	{
		if (RestVolume >= HalfStack *  CachedItem->GetItemDatas()->ItemVolume)
		{
			AC_Item* SpawnItem =  CachedItem->SpawnItem(PlayerCharacter);
			 CachedItem->SetItemStack( CachedItem->GetItemCurStack() - HalfStack);
			SpawnItem->SetItemStack(HalfStack);
			SpawnItem->MoveToInven(PlayerCharacter, HalfStack);
			return true;
		}
		else
		{
			CachedItem->MoveToInven(PlayerCharacter, HalfStack);
			return true;
		}

	}
	else if (CachedItem->GetItemPlace() == EItemPlace::INVEN)
	{
		AC_Item* SpawnItem =  CachedItem->SpawnItem(PlayerCharacter);
		 CachedItem->SetItemStack( CachedItem->GetItemCurStack() - HalfStack);

		SpawnItem->SetItemStack(HalfStack);

		float DividedItemVolume = PlayerCharacter->GetInvenComponent()->GetCurVolume() - SpawnItem->GetItemAllVolume();

		SpawnItem->SetActorEnableCollision(true);

		SpawnItem->MoveToAround(PlayerCharacter, HalfStack);

		PlayerCharacter->GetInvenComponent()->AddInvenCurVolume(-DividedItemVolume);

		PlayerCharacter->GetInvenSystem()->GetInvenUI()->UpdateVolumeBar(PlayerCharacter);
		return true;
	}

	return false;
}


