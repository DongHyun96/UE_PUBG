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


		// ���콺 �Է��� UI��, �ٸ� �Է��� ���ӿ��� ó��
		PC->SetInputMode
		(
			FInputModeGameAndUI()
			.SetWidgetToFocus(this->TakeWidget())
			.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock)
			.SetHideCursorDuringCapture(false)
		);

		PC->bShowMouseCursor = true;
		PC->SetIgnoreLookInput(true);  // ���콺 �̵��� ���� ī�޶� ȸ���� ����
		//PC->SetIgnoreMoveInput(true);  // ���콺 Ŭ���� ���� �������� ����
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

		PC->SetIgnoreLookInput(false);  // ���콺 �̵��� ���� ī�޶� ȸ���� ����
		//PC->SetIgnoreMoveInput(false);  // ���콺 Ŭ���� ���� �������� ����
	}

}

FReply UC_MainMapWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// ���� ���콺 ��ư Ŭ�� ó��
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		UC_Util::Print("MouseClicked", FColor::Red, 10.f);

		FVector2D ScreenSpacePosition = InMouseEvent.GetScreenSpacePosition();
		FVector2D LocalPosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

		UC_Util::Print(LocalPosition, FColor::Red);

		// ó�� �Ϸ� �� �Է��� �Һ��ϰ� ���ĵ��� �ʰ� ��
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply UC_MainMapWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	float ScrollDelta = InMouseEvent.GetWheelDelta();

	// ��ũ�� �Է� ó��
	FString str = "Mouse Wheel Scrolled: " + FString::SanitizeFloat(ScrollDelta);
	UC_Util::Print(str, FColor::Red, 10.f);

	if (!MainMapImg) return FReply::Handled();

	// +�� ���� �ø�
	MainMapScale += ScrollDelta * SCROLL_DELTA_STEP;
	MainMapScale = FMath::Clamp(MainMapScale, 1.f, MAP_SCALE_MAX);
	MainMapImg->SetRenderScale(FVector2D(MainMapScale, MainMapScale));


	return FReply::Handled();
}
