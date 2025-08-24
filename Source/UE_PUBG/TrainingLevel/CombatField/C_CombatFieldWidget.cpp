// Fill out your copyright notice in the Description page of Project Settings.


#include "C_CombatFieldWidget.h"

#include "C_PlayerCombatFieldWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Utility/C_Util.h"

void UC_CombatFieldWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!IsValid(CombatSimulationPanel)) UC_Util::Print("From UC_CombatFieldWidget : CombatSimulationPanel not valid!", FColor::Red, 10.f);
	if (!IsValid(SpectatorInfoText)) UC_Util::Print("From UC_CombatFieldWidget : SpectatorInfoText not valid!", FColor::Red, 10.f);

	if (!IsValid(PlayerCombatFieldWidget)) UC_Util::Print("From UC_CombatFieldWidget : PlayerCombatFieldWidget not valid!", FColor::Red, 10.f);
}

void UC_CombatFieldWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	HandleSpectatorInfo(InDeltaTime);
}

void UC_CombatFieldWidget::SetSpectatorInfoText(ESpectatorType InSpectatorType)
{
	if (!SpectatorInfoTextMap.Contains(InSpectatorType)) return;
	SpectatorInfoText->SetText(FText::FromString(SpectatorInfoTextMap[InSpectatorType]));
}

void UC_CombatFieldWidget::HandleSpectatorInfo(float InDeltaTime)
{
	if (CombatSimulationPanel->GetRenderOpacity() < 0.9f) return;

	const float SpectatingInfoPanelOpacity = FMath::Lerp(SpectatingInfoPanel->GetRenderOpacity(), SpectatingInfoPanelOpacityDestination, InDeltaTime * 10.f);
	SpectatingInfoPanel->SetRenderOpacity(SpectatingInfoPanelOpacity);

	if (SpectatingInfoPanelOpacity < 0.1f) return;

	static float TimeAccumulator{};
	TimeAccumulator += InDeltaTime;

	const float FlickerSpeed = 3.5f;

	const float Opacity = FMath::Sin(FlickerSpeed * TimeAccumulator) * 0.5f + 0.5f; // Sin값 0 ~ 1로 mapping
	SpectatorInfoText->SetRenderOpacity(Opacity);
}
