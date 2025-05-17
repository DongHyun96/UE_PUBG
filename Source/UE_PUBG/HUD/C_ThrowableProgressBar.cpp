// Fill out your copyright notice in the Description page of Project Settings.


#include "C_ThrowableProgressBar.h"

#include "Components/Image.h"

void UC_ThrowableProgressBar::OnParentWidgetNativeConstruct()
{
	this->SetVisibility(ESlateVisibility::Hidden);
}

void UC_ThrowableProgressBar::Tick(float InDeltaTime)
{
	if (ProgressBar->GetPercent() > 0.68f)
	{
		FLinearColor Color = {1.f, 0.f, 0.f, 1.f};
		ProgressBar->SetFillColorAndOpacity(Color);

		FlickerElapsedTime += InDeltaTime;

		if (FlickerElapsedTime >= FlickerInterval)
		{
			bIsFlickerImageVisible = !bIsFlickerImageVisible;

			ESlateVisibility NewVisibility = bIsFlickerImageVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden;
			WarningFlickerImage->SetVisibility(NewVisibility);

			FlickerElapsedTime = 0.f;
		}
	}
	else
	{
		FLinearColor Color = {1.f, 1.f, 1.f, 0.8f};
		ProgressBar->SetFillColorAndOpacity(Color);

		WarningFlickerImage->SetVisibility(ESlateVisibility::Hidden);
		bIsFlickerImageVisible = true;
		FlickerElapsedTime = 0.f;
	}
}
