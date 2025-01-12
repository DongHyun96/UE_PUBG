// Fill out your copyright notice in the Description page of Project Settings.

#include "InvenUserInterface/C_InvenUiWidget.h"

#include "InvenUserInterface/C_ItemBarWidget.h"
#include "InvenUserInterface/C_MainGunWidget.h"
#include "InvenUserInterface/C_ThrowableWidget.h"
#include "InvenUserInterface/C_EquipSlot.h"
#include "InvenUserInterface/C_EquipmentPanel.h"
#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_PlayerController.h"

#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/ListView.h"
#include "UE_PUBG/Character/C_BasicCharacter.h"
#include "C_MyItemListWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"

#include "TimerManager.h"

#include "Utility/C_Util.h"

#include "Kismet/GameplayStatics.h"

void UC_InvenUiWidget::NativeConstruct()
{
    Super::NativeConstruct();

    SetIsFocusable(false);
    
    if (InvenCanvas)
    {
        
    }
    
    InitWidget();

    InitListView();

    AC_PlayerController* PlayerController = Cast<AC_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

}

void UC_InvenUiWidget::SetVisibility(ESlateVisibility InVisibility)
{
    UUserWidget::SetVisibility(InVisibility);

    AC_PlayerController* PlayerController = Cast<AC_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    //PlayerController->SetIgnoreMoveInput(false);
    UC_Util::Print(PlayerController->GetCurrentInputModeDebugString(),FColor::Magenta,50.f);
    //PlayerController->SetIgnoreLookInput(false);
    //if (InVisibility == ESlateVisibility::HitTestInvisible) return;

    if (InVisibility == ESlateVisibility::Visible)
    {
        //if (!this->IsInViewport())
        //{
        //    this->AddToViewport();
        //    UC_Util::Print("Adding to viewport", FColor::Red, 10.f);
        //}

        //if (!GetIsPanelOpened() && (GetVisibility() == ESlateVisibility::HitTestInvisible)) return;

        //if (GetVisibility() == ESlateVisibility::HitTestInvisible)
        //{
        //    UUserWidget::SetVisibility(InVisibility);
        //    return;
        //}
	    //PlayerController->SetIgnoreLookInput(true);

        //FInputModeGameAndUI InputMode{};
	    //InputMode.SetWidgetToFocus(this->TakeWidget());
	    //InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        //InputMode.SetHideCursorDuringCapture(false);
	    //PlayerController->SetInputMode(InputMode);

        PlayerController->SetInputMode
        (
            FInputModeGameAndUI()
            .SetWidgetToFocus(nullptr)
            .SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock)
            .SetHideCursorDuringCapture(false)
        );
        PlayerController->GetPawn()->bUseControllerRotationYaw = false;
        //PlayerController->bEnableClickEvents = true;  // UI 클릭 이벤트 활성화
        //PlayerController->bEnableTouchEvents = true; // 터치 이벤트 활성화
        //PlayerController->bEnableMouseOverEvents = true; // 마우스 오버 이벤트 활성화
	    PlayerController->bShowMouseCursor = true;
        //PlayerController->SetIgnoreLookInput(true);
        //SetIsFocusable(true);

    }

    if (InVisibility == ESlateVisibility::Hidden)
    {
        PlayerController->GetPawn()->bUseControllerRotationYaw = true;
        PlayerController->SetInputMode(FInputModeGameOnly());
        //PlayerController->SetInputMode(FInputModeGameAndUI());

        PlayerController->bShowMouseCursor = false;
        PlayerController->SetIgnoreLookInput(false);
    }

    UC_Util::Print(PlayerController->GetCurrentInputModeDebugString(), FColor::Black, 50.f);

    //PlayerController->SetIgnoreMoveInput(false);

    //PlayerController->SetIgnoreLookInput(false);

}

bool UC_InvenUiWidget::GetIsPanelOpened()
{
    switch (this->GetVisibility())
    {
    case ESlateVisibility::Visible: case ESlateVisibility::SelfHitTestInvisible: case ESlateVisibility::HitTestInvisible: return true;
    default: return false;
    }
}

void UC_InvenUiWidget::InitWidget()
{

    SetWidgetsOwner(OwnerCharacter);
    
    InitListView();

    UpdateVolumeBar(OwnerCharacter);

    EquipmentPanel->InitializeWidget();

    if (!IsValid(MainGunSlot)) return;

    MainGunSlot->SetWeaponBoxNum(1);
    //MainGunSlot->Init();

    if (!IsValid(SubGunSlot)) return;
    SubGunSlot->SetWeaponBoxNum(2);
    //SubGunSlot->Init();

    if (!IsValid(MeleeSlot)) return;
    MeleeSlot->SetWeaponBoxNum(4);
    //MeleeSlot->Init();

    if (!IsValid(ThrowableSlot)) return;
    ThrowableSlot->SetWeaponBoxNum(5);
    //ThrowableSlot->Init();

    if (!IsValid(BackPackSlot)) return;
    BackPackSlot->Init();
}

void UC_InvenUiWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!GetIsPanelOpened()) return;

    if (!OwnerCharacter->GetIsActivatingConsumableItem()) return;
    
    //this->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UC_InvenUiWidget::SetWidgetsOwner(AC_BasicCharacter* Character)
{
    if (IsValid(MainGunSlot))
        MainGunSlot  ->SetOwnerCharacter(Cast<AC_Player>(Character));
    if (IsValid(SubGunSlot))
        SubGunSlot   ->SetOwnerCharacter(Cast<AC_Player>(Character));
    if (IsValid(MeleeSlot))
        MeleeSlot    ->SetOwnerCharacter(Cast<AC_Player>(Character));
    if (IsValid(ThrowableSlot))
        ThrowableSlot->SetOwnerCharacter(Cast<AC_Player>(Character));
    if (IsValid(BackPackSlot))
        BackPackSlot->SetOwnerCharacter(Cast<AC_Player>(Character));

    EquipmentPanel->SetOwnerChracter(Character);

    EquipmentPanel->SetWidgetOwnerCharacter(Character);
}

void UC_InvenUiWidget::InitListView()
{
    //dnseInitWidget();

    if (MyItemListWidget)
    {
        TMap<FString, TArray<AC_Item*>> MyItems; // 실제 아이템 리스트를 가져오는 로직 필요
        MyItems = OwnerCharacter->GetInvenComponent()->GetTestMyItems();
        MyItemListWidget->SetVisibility(ESlateVisibility::Visible);

        MyItemListWidget->AddTMapItem(MyItems); // 아이템 리스트 추가
        //PopulateItemList(MyItemListWidget->ItemListView, MyItems);
        //MyItemListWidget->AddToViewport();
    }

    if (IsValid(AroundItemListWidget))
    {
        //TMap<FString, AC_Item*> AroundItems; // 실제 아이템 리스트를 가져오는 로직 필요
        //AroundItems = OwnerCharacter->GetInvenComponent()->GetTestAroundItems();

        TArray<AC_Item*> TestAroundItemList;
        TestAroundItemList = OwnerCharacter->GetInvenComponent()->GetNearItems();
        AroundItemListWidget->SetVisibility(ESlateVisibility::Visible);
        //AroundItemListWidget->AddTMapItem(TestAroundItemList);
        //testAroundItemList(AroundItemListWidget->ItemListView, TestAroundItemList);
        AroundItemListWidget->InitAroundItemList(TestAroundItemList);
        //AroundItemListWidget->AddToViewport();
    }
}

void UC_InvenUiWidget::PopulateItemList(UListView* list, const TMap<FString, AC_Item*>& itemList)
{
    if (!IsValid(list))
    {
        UC_Util::Print("void List");
        return;
    }
    
    list->ClearListItems(); // 기존 아이템 삭제
    
    //if (itemList)

    if (!(itemList.Num() > 0)) return;
    for (const auto& ItemPair : itemList)
    {
         AC_Item* Item = ItemPair.Value; // TMap에서 아이템 가져오기
         if (IsValid(Item))
         {
             list->AddItem(Item);
         }
         else
         {
             return;
         }
         //ItemBar갱신.
         UC_ItemBarWidget* EntryWidget = Cast<UC_ItemBarWidget>(MyItemListView->GetEntryWidgetFromItem(Item));
         //UC_ItemBarWidget* EntryWidget = Cast<UC_ItemBarWidget>(MyItemListWidget->ItemListBar->GetEntryWidgetFromItem(Item));

         if (IsValid(EntryWidget))
             EntryWidget->InitBar(Item);
    }

    //list->RequestRefresh();
}

void UC_InvenUiWidget::testAroundItemList(UListView* list, const TArray<AC_Item*>& AroundItemList)
{
    if (!IsValid(list))
    {
        UC_Util::Print("void List");
        return;
    }

    list->ClearListItems(); // 기존 아이템 삭제

    if (!(AroundItemList.Num() > 0)) return;

    for (auto& AroundItem : AroundItemList)
    {
        AC_Item* Item = AroundItem;
        if (IsValid(Item))
        {
            list->AddItem(Item);
        }
        else
        {
            return;
        }
        //UC_ItemBarWidget* EntryWidget = Cast<UC_ItemBarWidget>(MyItemListView->GetEntryWidgetFromItem(Item));
        UC_ItemBarWidget* EntryWidget = Cast<UC_ItemBarWidget>(AroundItemListView->GetEntryWidgetFromItem(Item));

        if (EntryWidget)
            EntryWidget->InitBar(Item);
    }
}

void UC_InvenUiWidget::UpdateVolumeBar(AC_BasicCharacter* Character)
{
    float curVolume = Character->GetInvenComponent()->GetCurVolume();
    float maxVolume = Character->GetInvenComponent()->GetMaxVolume();

    float MaxVolumePercent = maxVolume / 370.f; //370 = 70(기본) + 250(3LVBackPack) + 50(갑빠)
    float curVolumePercent = curVolume / 370.f;

    curVolumeBar->SetPercent(curVolumePercent);
    maxVolumeBar->SetPercent(MaxVolumePercent);
}


