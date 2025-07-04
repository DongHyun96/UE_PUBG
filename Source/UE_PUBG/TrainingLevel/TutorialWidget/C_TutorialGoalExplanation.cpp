// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TutorialGoalExplanation.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Utility/C_Util.h"

void UC_TutorialGoalExplanation::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Init ExplanationPanels
	ExplanationPanels.Empty();

	for (int i = 0;; ++i)
	{
		FString ExplanationPanelStr = "ExplanationPanel" + FString::FromInt(i);
		FName WidgetName(*ExplanationPanelStr);
		
		if (UCanvasPanel* ExplanationPanel = Cast<UCanvasPanel>(GetWidgetFromName(WidgetName)))
		{
			LastIndex = i;
			ExplanationPanel->SetRenderOpacity(0.f);
			ExplanationPanels.Add(ExplanationPanel);
		}
		else break;
	}

	// Init BackgroundImages
	BackgroundImages.Empty();
	
	uint8 ImageLastIdx{};
	for (int i = 0;; ++i)
	{
		FString BackgroundImageStr = "BackgroundImage" + FString::FromInt(i);
		FName WidgetName(*BackgroundImageStr);
		
		if (UImage* BackgroundImage = Cast<UImage>(GetWidgetFromName(WidgetName)))
		{
			BackgroundImage->SetRenderScale(FVector2D(0.f, 1.f));
			BackgroundImages.Add(BackgroundImage);
			ImageLastIdx = i;
		}
		else break;
	}

	if (ImageLastIdx != LastIndex)
	{
		FString Str =	"From " +
						this->GetFName().ToString() +
						" NativeConstruct : BackgroundImage Num and ExplanationPanel Num not matching!";
		UC_Util::Print(Str, FColor::Red, 10.f);
	}

	CurrentActivatedIndex = 0;
	this->SetVisibility(ESlateVisibility::Hidden);
}

void UC_TutorialGoalExplanation::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	/* Render Opacity & Render Scale 업데이트 */
	for (int i = 0; i < LastIndex + 1; ++i)
	{
		// RenderOpacity와 RenderScale 모두 0.f - 1.f의 범위 값을 가지므로 동일한 Lerp Dest값 사용
		const float LerpDestination = (i == CurrentActivatedIndex) ? 1.f : 0.f;
		
		const float NewOpacity = FMath::Lerp(ExplanationPanels[i]->GetRenderOpacity(), LerpDestination, 5.f * InDeltaTime);
		ExplanationPanels[i]->SetRenderOpacity(NewOpacity);

		const float NewScaleX = FMath::Lerp(BackgroundImages[i]->GetRenderTransform().Scale.X, LerpDestination, 5.f * InDeltaTime);
		BackgroundImages[i]->SetRenderScale(FVector2D(NewScaleX, 1.f));
	}

	/* 총 활성화 시간 체크 */
	
	static float ActivationTotalTimeLimit = 4.f; // 총 활성화 시간(마지막 설명문이 아니라면) -> 총 4초

	if (CurrentActivatedIndex == LastIndex) return;

	ActivatedTimer += InDeltaTime;

	if (ActivatedTimer > ActivationTotalTimeLimit) // 다음 설명문으로 넘어가기
	{
		ActivatedTimer = 0.f;
		++CurrentActivatedIndex;
	}
}

void UC_TutorialGoalExplanation::StartExplanation()
{
	this->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	// Initial 값들로 초기화 처리
	
	CurrentActivatedIndex = 0;
	ActivatedTimer = 0.f;

	for (UCanvasPanel* ExplanationPanel : ExplanationPanels)
		ExplanationPanel->SetRenderOpacity(0.f);

	for (UImage* BackgroundImage : BackgroundImages)
		BackgroundImage->SetRenderScale(FVector2D(0.f, 1.f));
}

void UC_TutorialGoalExplanation::StopExplanation()
{
	this->SetVisibility(ESlateVisibility::Hidden);
}
