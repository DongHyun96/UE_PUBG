// Fill out your copyright notice in the Description page of Project Settings.


#include "C_CombatFieldWidget.h"

#include "Components/CanvasPanel.h"
#include "Utility/C_Util.h"

void UC_CombatFieldWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!IsValid(CombatSimulationPanel))
		UC_Util::Print("From UC_CombatFieldWidget : CombatSimluationPanel not valid!", FColor::Red, 10.f);
}

void UC_CombatFieldWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}
