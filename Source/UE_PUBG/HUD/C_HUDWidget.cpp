﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/C_HUDWidget.h"

#include "C_MagneticFieldIndicatorWidget.h"
#include "C_ThrowableProgressBar.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"


const FVector2D UC_HUDWidget::MINIMAP_MINIMIZED_SIZE = { 300.f, 300.f };
const FVector2D UC_HUDWidget::MINIMAP_MINIMIZED_POS = { -175.921875f, -175.921875f };

const FVector2D UC_HUDWidget::MINIMAP_ENLARGED_SIZE = { 600.f, 600.f };
const FVector2D UC_HUDWidget::MINIMAP_ENLARGED_POS = { -329.921875f, -325.081055f };

void UC_HUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GAMESCENE_MANAGER->SetHUDWidgetByHUDMode(EHUDMode::IDLE, this);

	MagneticFieldIndicatorWidget = Cast<UC_MagneticFieldIndicatorWidget>(GetWidgetFromName(TEXT("WBPC_MagneticFieldIndicator")));
	if (!MagneticFieldIndicatorWidget) UC_Util::Print("From HUDWidget::NativeConstruct : MagneticFieldIndicatorWidget not inted!", FColor::Red, 10.f);
	MagneticFieldIndicatorPanelSlot = Cast<UCanvasPanelSlot>(MagneticFieldIndicatorWidget->Slot);

	// Set ThrowableProgressBar
	ThrowableProgressBar = Cast<UC_ThrowableProgressBar>(GetWidgetFromName(TEXT("ThrowableProgressBarPanel")));
	if (!ThrowableProgressBar)
	{
		UC_Util::Print("From UC_HUDWidget::NativeConstruct : ThrowableProgressBar nullptr!", FColor::Red, 10.f);
		return;
	}
	
	UImage* ThrowableProgressBarWarningFlickerImage = Cast<UImage>(GetWidgetFromName(TEXT("WarningFlickerImage")));
	if (!ThrowableProgressBarWarningFlickerImage)
		UC_Util::Print("From UC_HUDWidget::NativeConstruct : ThrowableProgressBarWarningFlickerImage nullptr!", FColor::Red, 10.f);
	ThrowableProgressBar->SetWarningFlickerImage(ThrowableProgressBarWarningFlickerImage);

	UProgressBar* ThrowableProgressBarOrigin = Cast<UProgressBar>(GetWidgetFromName(TEXT("RedProgressBar")));
	if (!ThrowableProgressBarOrigin)
		UC_Util::Print("From UC_HUDWidget::NativeConstruct : ThrowableProgressBarOrigin nullptr!", FColor::Red, 10.f);
	ThrowableProgressBar->SetProgressBar(ThrowableProgressBarOrigin);

	ThrowableProgressBar->OnParentWidgetNativeConstruct();
}

void UC_HUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Handle Lerp MagneticFieldIndicator Pos
	FVector2D MagneticFieldIndicatorPos = FMath::Lerp(MagneticFieldIndicatorPanelSlot->GetPosition(), MagneticFieldIndicatorPosLerpDestination, InDeltaTime * 10.f);
	MagneticFieldIndicatorPanelSlot->SetPosition(MagneticFieldIndicatorPos);

	// Throwable ProgressBar Tick (Custom Tick)
	if (ThrowableProgressBar->GetVisibility() == ESlateVisibility::Hidden || ThrowableProgressBar->GetVisibility() == ESlateVisibility::Collapsed)
		return;
	ThrowableProgressBar->Tick(InDeltaTime);
}

void UC_HUDWidget::ToggleMiniMapEnlarged()
{
	bIsMiniMapEnlarged = !bIsMiniMapEnlarged;

	MiniMapLerpSizeDest = (bIsMiniMapEnlarged) ? MINIMAP_ENLARGED_SIZE : MINIMAP_MINIMIZED_SIZE;
	MiniMapLerpPosDest	= (bIsMiniMapEnlarged) ? MINIMAP_ENLARGED_POS  : MINIMAP_MINIMIZED_POS;
	MagneticFieldIndicatorPosLerpDestination = (bIsMiniMapEnlarged) ? MINIMAP_ENLARGED_INDICATOR_POS : MINIMAP_MINIMIZED_INDICATOR_POS;
}


