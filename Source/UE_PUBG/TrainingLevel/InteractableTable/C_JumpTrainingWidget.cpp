// Fill out your copyright notice in the Description page of Project Settings.


#include "C_JumpTrainingWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"

void UC_JumpTrainingWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UC_JumpTrainingWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	HandleSpectatorInfo(InDeltaTime);
}

void UC_JumpTrainingWidget::HandleSpectatorInfo(float InDeltaTime)
{
	if (RenderOpacity < 0.9f) return;
	
	const float SpectatingInfoPanelOpacity = FMath::Lerp(SpectatingInfoPanel->GetRenderOpacity(), SpectatingInfoPanelOpacityDestination, InDeltaTime * 10.f);
	SpectatingInfoPanel->SetRenderOpacity(SpectatingInfoPanelOpacity);

	if (SpectatingInfoPanelOpacity < 0.1f) return;

	static float TimeAccumulator{};
	TimeAccumulator += InDeltaTime;

	const float FlickerSpeed = 3.5f;

	const float Opacity = FMath::Sin(FlickerSpeed * TimeAccumulator) * 0.5f + 0.5f; // Sin값 0 ~ 1로 mapping
	SpectatorInfoText->SetRenderOpacity(Opacity);
}
