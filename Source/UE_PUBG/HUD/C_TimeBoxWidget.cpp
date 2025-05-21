// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TimeBoxWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"

void UC_TimeBoxWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetAliveCountText(GAMESCENE_MANAGER->GetTotalPlayedCharacterCount());
	if (!GAMESCENE_MANAGER->GetAirplaneManager()) SetAliveText("ALIVE");
}

void UC_TimeBoxWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	float Opacity = FMath::Lerp(KilledCanvasPanel->GetRenderOpacity(), KilledCanvasOpacityLerpDest, InDeltaTime * 5.f);
	KilledCanvasPanel->SetRenderOpacity(Opacity);
}

void UC_TimeBoxWidget::SetAliveText(const FString& InText)
{
	AliveText->SetText(FText::FromString(InText));
}

void UC_TimeBoxWidget::SetAliveCountText(int InCount)
{
	AliveCountText->SetText(FText::FromString(FString::FromInt(InCount)));
	PlayAnimation(AliveCountTextAnimation);
}

void UC_TimeBoxWidget::SetKilledCountText(int InCount)
{
	FString str = "Setting KillCount : " + FString::FromInt(InCount);
	UC_Util::Print(str, FColor::Red, 10.f);
	
	KilledCountText->SetText(FText::FromString(FString::FromInt(InCount)));
}

void UC_TimeBoxWidget::ToggleKilledCountTextVisibility(bool InVisible)
{
	KilledCanvasOpacityLerpDest = InVisible ? 1.f : 0.f;
}
