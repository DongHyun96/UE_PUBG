// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/C_BloodScreenWidget.h"
#include "C_BloodScreenWidget.h"

void UC_BloodScreenWidget::ShowHitEffect()
{
	DynamicOpacity = 1.0f;
	SetVisibility(ESlateVisibility::HitTestInvisible);
}
