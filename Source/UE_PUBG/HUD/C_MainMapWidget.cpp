// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/C_MainMapWidget.h"
#include "HUD/C_HUDWidget.h"

#include "Singleton/C_GameSceneManager.h"
#include "Character/C_Player.h"
#include "Utility/C_Util.h"

#include "Components/Border.h"

#include "Kismet/GameplayStatics.h"

void UC_MainMapWidget::OnMKey()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (!bInputActionSettingInited)
	{
		// 입력 모드를 설정하여 위젯에서 입력을 받도록 함
		bInputActionSettingInited = true;
	}


	
	// Toggle Visibility of MainMap and MiniMap
	if (this->Visibility == ESlateVisibility::Hidden)
	{
		SetVisibility(ESlateVisibility::Visible);
		GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetMiniMapWidget()->SetVisibility(ESlateVisibility::Hidden);
		GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetMiniMapBorder()->SetVisibility(ESlateVisibility::Hidden);

		PC->SetInputMode(FInputModeUIOnly().SetWidgetToFocus(this->TakeWidget()));
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeGameAndUI().SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock));
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
		GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetMiniMapWidget()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetMiniMapBorder()->SetVisibility(ESlateVisibility::Visible);

		PC->bShowMouseCursor = false;
		//PC->SetInputMode(FInputModeGameAndUI().SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen));
		PC->SetInputMode(FInputModeGameOnly());
	}

}
