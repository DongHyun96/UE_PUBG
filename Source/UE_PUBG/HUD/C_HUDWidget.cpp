// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/C_HUDWidget.h"

#include "Utility/C_Util.h"

void UC_HUDWidget::ToggleMiniMapEnlarged()
{
	bIsMiniMapEnlarged = !bIsMiniMapEnlarged;

	MiniMapLerpSizeDest = (bIsMiniMapEnlarged) ? MINIMAP_ENLARGED_SIZE : MINIMAP_MINIMIZED_SIZE;
	MiniMapLerpPosDest	= (bIsMiniMapEnlarged) ? MINIMAP_ENLARGED_POS  : MINIMAP_MINIMIZED_POS;
}
