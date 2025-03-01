// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUI/C_InventoryUIWidget.h"

#include "InvenUI/Panel/ItemPanel/C_AroundItemPanelWidget.h"
#include "InvenUI/Panel/ItemPanel/C_InventoryItemPanelWidget.h"

#include "InvenUI/BasicItemSlot/EquipmentSlot/C_EquipmentSlotWidget.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/GunSlot/C_GunSlotWidget.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/C_MeleeWeaponSlotWidget.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/C_ThrowableWeaponSlotWidget.h"

#include "InvenUI/Panel/ItemPanel/EquipmentPanel/C_EquipmentPanelWdiget.h"
#include "InvenUI/Panel/DivideItemPanel/C_BasicDivideItemPanelWidget.h"

#include "Components/ProgressBar.h"

#include "Kismet/GameplayStatics.h"

#include "Character/Component/C_PlayerController.h"

void UC_InventoryUIWidget::SetVisibility(ESlateVisibility InVisibility)
{
    UUserWidget::SetVisibility(InVisibility);

    AC_PlayerController* PlayerController = Cast<AC_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    //PlayerController->SetIgnoreMoveInput(false);
    //UC_Util::Print(PlayerController->GetCurrentInputModeDebugString(),FColor::Magenta,50.f);
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
            .SetWidgetToFocus(this->TakeWidget())
            .SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock)
            .SetHideCursorDuringCapture(false)
        );
        PlayerController->bShowMouseCursor = true;
        PlayerController->SetIgnoreLookInput(true);
        //PlayerController->GetPawn()->bUseControllerRotationYaw = false;
        //PlayerController->bEnableClickEvents = true;  // UI 클릭 이벤트 활성화
        //PlayerController->bEnableTouchEvents = true; // 터치 이벤트 활성화
        //PlayerController->bEnableMouseOverEvents = true; // 마우스 오버 이벤트 활성화
        //SetIsFocusable(true);

    }

    if (InVisibility == ESlateVisibility::Hidden)
    {
        //PlayerController->GetPawn()->bUseControllerRotationYaw = true;
        PlayerController->SetInputMode(FInputModeGameOnly());
        //PlayerController->SetInputMode(FInputModeGameAndUI());

        PlayerController->bShowMouseCursor = false;
        PlayerController->SetIgnoreLookInput(false);
    }
}

void UC_InventoryUIWidget::SetOwnerPlayer(AC_Player* InOwnerPlayer)
{
    OwnerPlayer = InOwnerPlayer;

    if (AroundItemPanel)
        AroundItemPanel->SetOwnerPlayer(InOwnerPlayer);

    if (InventoryPanel)
        InventoryPanel->SetOwnerPlayer(InOwnerPlayer);

    if (EquipmentPanel)
        EquipmentPanel->SetOwnerPlayer(InOwnerPlayer);

    if (DivideItemAroundDropWidget)
        DivideItemAroundDropWidget->SetOwnerPlayer(InOwnerPlayer);

    if (DivideItemInventoryDropWidget)
        DivideItemInventoryDropWidget->SetOwnerPlayer(InOwnerPlayer);
}

//void UC_InventoryUIWidget::SetWidgetsOwner(AC_BasicCharacter* Character)
//{
//    //if (AroundItemPanel)
//    //    AroundItemPanel->SetOwnerPlayer(InOwnerPlayer);
//    //
//    //if (InventoryPanel)
//    //    InventoryPanel->SetOwnerPlayer(InOwnerPlayer);
//    //
//    //if (EquipmentPanel)
//    //    EquipmentPanel->SetOwnerPlayer(InOwnerPlayer);
//}

void UC_InventoryUIWidget::UpdateVolumeBar(AC_BasicCharacter* Character)
{
    float curVolume = Character->GetInvenComponent()->GetCurVolume();
    float maxVolume = Character->GetInvenComponent()->GetMaxVolume();

    float MaxVolumePercent = maxVolume / 370.f; //370 = 70(기본) + 250(3LVBackPack) + 50(갑빠)
    float curVolumePercent = curVolume / 370.f;

    CurVolumeBar->SetPercent(curVolumePercent);
    MaxVolumeBar->SetPercent(MaxVolumePercent);
}

void UC_InventoryUIWidget::UpdateWidget()
{
    EquipmentPanel->UpdateWidget();

    UpdateVolumeBar(OwnerPlayer);

    UpdateAroundItemPanelWidget();

    UpdateInventroyItemPanelWidget();
}

bool UC_InventoryUIWidget::GetIsPanelOpened()
{
    switch (this->GetVisibility())
    {
    case ESlateVisibility::Visible: case ESlateVisibility::SelfHitTestInvisible: case ESlateVisibility::HitTestInvisible: return true;
    default: return false;
    }
}

void UC_InventoryUIWidget::UpdateAroundItemPanelWidget()
{
    if (IsValid(AroundItemPanel))
    {
        //TMap<FString, AC_Item*> AroundItems; // 실제 아이템 리스트를 가져오는 로직 필요
        TArray<AC_Item*> TestAroundItemList;
        TestAroundItemList = OwnerPlayer->GetInvenComponent()->GetTestAroundItems();
        AroundItemPanel->SetVisibility(ESlateVisibility::Visible);

        AroundItemPanel->InitializeItemList(TestAroundItemList);
    }
}

void UC_InventoryUIWidget::UpdateInventroyItemPanelWidget()
{
    if (InventoryPanel)
    {
        TMap<FString, AC_Item*> MyItems; // 실제 아이템 리스트를 가져오는 로직 필요
        MyItems = OwnerPlayer->GetInvenComponent()->GetTestMyItems();
        InventoryPanel->SetVisibility(ESlateVisibility::Visible);

        //InventoryPanel->AddTMapItem(MyItems); // 아이템 리스트 추가
        InventoryPanel->UpdateMyItemList(MyItems);
    }
}

void UC_InventoryUIWidget::MainPanelsSetVisivility(ESlateVisibility InVisibility)
{
    AroundItemPanel->SetVisibility(InVisibility);
     InventoryPanel->SetVisibility(InVisibility);
     EquipmentPanel->SetVisibility(InVisibility);
}

void UC_InventoryUIWidget::CloseDivideItemWidget()
{
    if (DivideItemAroundDropWidget->Visibility == ESlateVisibility::Visible)
        DivideItemAroundDropWidget->SetVisibility(ESlateVisibility::Collapsed);
    if (DivideItemInventoryDropWidget->Visibility == ESlateVisibility::Visible)
        DivideItemInventoryDropWidget->SetVisibility(ESlateVisibility::Collapsed);
}


