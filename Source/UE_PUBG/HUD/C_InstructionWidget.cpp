// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/C_InstructionWidget.h"

#include "SkeletalMeshAttributes.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Utility/C_Util.h"

void UC_InstructionWidget::NativeConstruct()
{
    Super::NativeConstruct();

    for (int i = 0; i < 4; ++i)
    {
        FString PlayerWarningWidgetName = FString::Printf(TEXT("PlayerWarningLogText%d"), i);
        FName WidgetName(*PlayerWarningWidgetName);
        UTextBlock* TextBlock = Cast<UTextBlock>(GetWidgetFromName(WidgetName));
        if (!TextBlock)
        {
            UC_Util::Print("From Instruction Widget NativeConstruct : TextBlock casting failed!", FColor::Red, 10.f);
            continue;
        }
 
        UCanvasPanelSlot* PlayerWarningLogTextPanel = Cast<UCanvasPanelSlot>(TextBlock->Slot);
        
        PlayerWarningLogTexts.Add(TextBlock);
        PlayerWarningLogTextPanels.Add(PlayerWarningLogTextPanel);
        PlayerWarningLogEachPositions.Add(PlayerWarningLogTextPanel->GetPosition());
    }
    
    PlayerWarningLogSequence = {0, 1, 2, 3};
}

void UC_InstructionWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

    HandleConsumableInstructionFlicker(InDeltaTime);
    HandleTextBlockFadeOut(InDeltaTime);

    HandlePlayerWarningLogLifeTimes(InDeltaTime);
    HandlePlayerWarningLogPositionsAndDefaultAlpha(InDeltaTime);
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

void UC_InstructionWidget::HandleTextBlockFadeOut(const float& DeltaTime)
{
    for (TSet<UTextBlock*>::TIterator It(FadeOutTextBlocks); It; ++It)
    {
        UTextBlock* TextBlock = *It;
        float CurrentAlpha = TextBlock->ColorAndOpacity.GetSpecifiedColor().A;

        FLinearColor ShadowColor = TextBlock->GetShadowColorAndOpacity();
        float ShadowAlpha = ShadowColor.A;
        
        if (CurrentAlpha < 0.05f) // Alpha가 충분히 작아서 Destination에 도달했다고 판단
        {
            TextBlock->SetOpacity(0.f);
            ShadowColor.A = 0.f;
            TextBlock->SetShadowColorAndOpacity(ShadowColor);            
            
            It.RemoveCurrent();
            continue;
        }

        // Lerp Alpha color to 0.f
        float DestAlpha = FMath::Lerp(CurrentAlpha, 0.f, DeltaTime * 10.f);
        TextBlock->SetOpacity(DestAlpha);
        
        ShadowAlpha = FMath::Lerp(ShadowAlpha, 0.f, DeltaTime * 15.f);
        ShadowColor.A = ShadowAlpha;
        TextBlock->SetShadowColorAndOpacity(ShadowColor);
    }
}

void UC_InstructionWidget::HandlePlayerWarningLogLifeTimes(const float& DeltaTime)
{
    for (TMap<UTextBlock*, float>::TIterator It(PlayerWarningLogLifeTimes); It; ++It)
    {
        TPair<UTextBlock*, float>& TextBlockLifeTimePair = *It;
        TextBlockLifeTimePair.Value -= DeltaTime;

        if (TextBlockLifeTimePair.Value <= 0.f) // 수명이 다한 TextBlockLifeTimePair
        {
            StartFadeOut(TextBlockLifeTimePair.Key);
            It.RemoveCurrent();
        }
    }
}

void UC_InstructionWidget::HandlePlayerWarningLogPositionsAndDefaultAlpha(const float& DeltaTime)
{
    for (int i = 1; i < PlayerWarningLogSequence.Num(); ++i)
    {
        int Index = PlayerWarningLogSequence[i];
        FVector2D CurrentPosition = PlayerWarningLogTextPanels[Index]->GetPosition();
        FVector2D DestPosition = FMath::Lerp(CurrentPosition, PlayerWarningLogEachPositions[i], 20.f * DeltaTime);
        PlayerWarningLogTextPanels[Index]->SetPosition(DestPosition);
    }

    // 3번째 로그는 보이는데, FadeOut중이 아니라면 FadeOut 처리
    int ThirdIndex = PlayerWarningLogSequence[2];
    if (PlayerWarningLogTexts[ThirdIndex]->GetColorAndOpacity().GetSpecifiedColor().A > 0.f) // 아직 보이는 중이고,
    {
        if (!FadeOutTextBlocks.Contains(PlayerWarningLogTexts[ThirdIndex])) // Start Fade 처리가 안된 상황
        {
            PlayerWarningLogLifeTimes.Remove(PlayerWarningLogTexts[ThirdIndex]);
            StartFadeOut(PlayerWarningLogTexts[ThirdIndex]);
        }
    }
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

bool UC_InstructionWidget::AddPlayerWarningLog(const FString& WarningLog)
{
    // 새로운 첫 번째 로그로 맨 뒤 로그 이동시키기
    int TargetIndex = PlayerWarningLogSequence.Last();
    PlayerWarningLogSequence.RemoveAt(PlayerWarningLogSequence.Num() - 1);
    PlayerWarningLogSequence.Insert(TargetIndex, 0);

    UTextBlock* TargetTextBlock = PlayerWarningLogTexts[TargetIndex]; 

    // 내용 setting 하기
    TargetTextBlock->SetText(FText::FromString(WarningLog));

    // Color Alpha값 기본 값으로 지정
    TargetTextBlock->SetOpacity(0.75f);

    // Shadow Alpha값도 기본 값으로 지정
    FLinearColor ShadowColor = TargetTextBlock->GetShadowColorAndOpacity();
    ShadowColor.A = 0.5f;
    TargetTextBlock->SetShadowColorAndOpacity(ShadowColor);

    // 초기 위치 지정
    PlayerWarningLogTextPanels[TargetIndex]->SetPosition(PlayerWarningLogEachPositions[0]);
    
    FTimerHandle TimerHandle{};

    // Maximum 3초의 Log LifeTime 처리
    PlayerWarningLogLifeTimes.Add(TargetTextBlock, 3.f);
    FadeOutTextBlocks.Remove(TargetTextBlock);
    
    return true;
}


