// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/C_HUDWidget.h"

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
}

void UC_HUDWidget::ToggleMiniMapEnlarged()
{
	bIsMiniMapEnlarged = !bIsMiniMapEnlarged;

	MiniMapLerpSizeDest = (bIsMiniMapEnlarged) ? MINIMAP_ENLARGED_SIZE : MINIMAP_MINIMIZED_SIZE;
	MiniMapLerpPosDest	= (bIsMiniMapEnlarged) ? MINIMAP_ENLARGED_POS  : MINIMAP_MINIMIZED_POS;
}


