// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/C_MainMapWidget.h"
#include "HUD/C_HUDWidget.h"

#include "Singleton/C_GameSceneManager.h"
#include "Character/C_Player.h"
#include "Utility/C_Util.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

void UC_MainMapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;
	bIsEnabled = true;
}

void UC_MainMapWidget::OnMKey()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// Toggle Visibility of MainMap and MiniMap
	if (this->Visibility == ESlateVisibility::Hidden)
	{
		SetVisibility(ESlateVisibility::Visible);
		GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetMiniMapWidget()->SetVisibility(ESlateVisibility::Hidden);
		GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetMiniMapBorder()->SetVisibility(ESlateVisibility::Hidden);


		// 마우스 입력은 UI로, 다른 입력은 게임에서 처리
		PC->SetInputMode
		(
			FInputModeGameAndUI()
			.SetWidgetToFocus(this->TakeWidget())
			.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock)
			.SetHideCursorDuringCapture(false)
		);

		PC->bShowMouseCursor = true;
		PC->SetIgnoreLookInput(true);  // 마우스 이동에 의한 카메라 회전을 막음
		//PC->SetIgnoreMoveInput(true);  // 마우스 클릭에 의한 움직임을 막음
		//PC->SetMouseCursorWidget()

		this->SetUserFocus(PC);

		if (this->HasUserFocus(PC)) UC_Util::Print("Focused", FColor::Red, 10.f);
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
		GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetMiniMapWidget()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetMiniMapBorder()->SetVisibility(ESlateVisibility::Visible);

		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());

		PC->SetIgnoreLookInput(false);  // 마우스 이동에 의한 카메라 회전을 막음
		//PC->SetIgnoreMoveInput(false);  // 마우스 클릭에 의한 움직임을 막음
	}

}

FReply UC_MainMapWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 왼쪽 마우스 버튼 클릭 처리
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		UC_Util::Print("MouseClicked", FColor::Red, 10.f);

		FVector2D ScreenSpacePosition = InMouseEvent.GetScreenSpacePosition();
		FVector2D LocalPosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

		UC_Util::Print(LocalPosition, FColor::Red);

		// 처리 완료 후 입력을 소비하고 전파되지 않게 함
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply UC_MainMapWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	float ScrollDelta = InMouseEvent.GetWheelDelta();

	// 스크롤 입력 처리
	FString str = "Mouse Wheel Scrolled: " + FString::SanitizeFloat(ScrollDelta);
	UC_Util::Print(str, FColor::Red, 10.f);

	if (!MainMapImg) return FReply::Handled();

	// +가 위로 올림
	MainMapScale += ScrollDelta * SCROLL_DELTA_STEP;
	MainMapScale = FMath::Clamp(MainMapScale, 1.f, MAP_SCALE_MAX);
	MainMapImg->SetRenderScale(FVector2D(MainMapScale, MainMapScale));


	return FReply::Handled();
}
