// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/C_MapWidget.h"

#include "Utility/C_Util.h"

bool UC_MapWidget::SpawnPingImage(FVector WorldPingLocation)
{
	return SpawnPingImageWithWorldPingLocation(WorldPingLocation);
}

bool UC_MapWidget::SpawnPingImage(FVector2D MousePos)
{
	return SpawnPingImageWithMousePos(MousePos);
}
