// Fill out your copyright notice in the Description page of Project Settings.


#include "C_PlayerCombatFieldWidget.h"

#include "Components/HorizontalBox.h"
#include "Utility/C_Util.h"

void UC_PlayerCombatFieldWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!IsValid(StartFKeyInstructionBox))
		UC_Util::Print("From UC_PlayerCombatFieldWidget::NativeConstruct : StartFKeyInstruction Widget Binding failed!", FColor::Red, 10.f);
	else StartFKeyInstructionBox->SetVisibility(ESlateVisibility::Hidden);
}

void UC_PlayerCombatFieldWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UC_PlayerCombatFieldWidget::ToggleStartFKeyInstruction(bool Visible)
{
	if (!IsValid(StartFKeyInstructionBox))
	{
		UC_Util::Print("From UC_PlayerCombatFieldWidget::ToggleStartFKeyInstruction : Invalid StartFKeyInstruction!", FColor::Red, 10.f);
		return;
	}

	StartFKeyInstructionBox->SetVisibility(Visible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
}
