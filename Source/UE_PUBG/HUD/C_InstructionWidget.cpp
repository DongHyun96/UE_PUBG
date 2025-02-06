// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/C_InstructionWidget.h"

#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"

void UC_InstructionWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

    HandleConsumableInstructionFlicker(InDeltaTime);
}

void UC_InstructionWidget::HandleConsumableInstructionFlicker(const float& DeltaTime)
{
    if (ConsumableInstruction->GetVisibility() == ESlateVisibility::Hidden) return;

    FLinearColor CurrentColor = ConsumableCurrentUsingTextBlock->ColorAndOpacity.GetSpecifiedColor();

    static float TimeAccumulator{};
    TimeAccumulator += DeltaTime;

    const float MinAlpha        = 0.1f;
    const float MaxAlpha        = 1.f;
    const float FlickerSpeed    = 5.f;

    CurrentColor.A = MinAlpha + (MaxAlpha - MinAlpha) * (0.5f + 0.5f * FMath::Sin(FlickerSpeed * TimeAccumulator));

    ConsumableCurrentUsingTextBlock->SetColorAndOpacity(FSlateColor(CurrentColor));
}

bool UC_InstructionWidget::ActivateConsumableInstruction(FString UsingTextContent)
{
    // 이미 다른 아이템 TextContent가 활성화 되어있을 때
    if (ConsumableInstruction->GetVisibility() == ESlateVisibility::SelfHitTestInvisible) return false;

    ConsumableInstruction->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    ConsumableCurrentUsingTextBlock->SetText(FText::FromStringView(UsingTextContent));

    // Set instruction alpha color to default
    FLinearColor Color = ConsumableCurrentUsingTextBlock->ColorAndOpacity.GetSpecifiedColor();
    Color.A = 1.f;

    ConsumableCurrentUsingTextBlock->SetColorAndOpacity(FSlateColor(Color));
    return true;
}


