// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/C_InstructionWidget.h"

#include "SkeletalMeshAttributes.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Character/Component/C_StatComponent.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/MeleeWeapon/C_MeleeWeapon.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"

void UC_InstructionWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Init PlayerWarningLog 관련
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

        PlayerWarningLogSequence.Add(i);
    }

    // Init Top KillLog 관련
    for (int i = 0; i < 7; ++i)
    {
        FTopKillFeedBlock Block{};

        /* Main Frame Panel & Panel Slot */
        FString Name = FString::Printf(TEXT("KillLog%d"), i);
        FName WidgetName(*Name);
        Block.Panel     = Cast<UCanvasPanel>(GetWidgetFromName(WidgetName));
        Block.PanelSlot = Cast<UCanvasPanelSlot>(Block.Panel->Slot);

        /* Killer Number */
        Name = FString::Printf(TEXT("KillerNumberPanel_%d"), i);
        WidgetName = *Name;
        Block.KillerNumberPanel = Cast<UCanvasPanel>(GetWidgetFromName(WidgetName));
        Name = FString::Printf(TEXT("KillerNumber_%d"), i);
        WidgetName = *Name;
        Block.KillerNumberText = Cast<UTextBlock>(GetWidgetFromName(WidgetName));

        /* Killer Text */
        Name = FString::Printf(TEXT("KillerName_%d"), i);
        WidgetName = *Name;
        Block.KillerText = Cast<UTextBlock>(GetWidgetFromName(WidgetName));

        /* KillFeedImage */
        Name = FString::Printf(TEXT("KillFeedImage_%d"), i);
        WidgetName = *Name;
        Block.KillFeedImage = Cast<UImage>(GetWidgetFromName(WidgetName));
        Block.KillFeedImagePanelSlot = Cast<UCanvasPanelSlot>(Block.KillFeedImage->Slot);

        /* HeadShot Image */
        Name = FString::Printf(TEXT("HeadShotImage_%d"), i);
        WidgetName = *Name;
        Block.HeadShotImage = Cast<UImage>(GetWidgetFromName(WidgetName));

        /* Victim Text */
        Name = FString::Printf(TEXT("VictimName_%d"), i);
        WidgetName = *Name;
        Block.VictimText = Cast<UTextBlock>(GetWidgetFromName(WidgetName));

        /* Victim Number */
        Name = FString::Printf(TEXT("VictimNumber_%d"), i);
        WidgetName = *Name;
        Block.VictimNumberText = Cast<UTextBlock>(GetWidgetFromName(WidgetName));

        TopKillFeedBlocks.Add(Block);
        TopKillFeedLogEachPositions.Add(Block.PanelSlot->GetPosition());
        TopKillFeedLogSequence.Add(i);
    }

    // Init Middle Kill Feed 관련
    MiddleKillFeedPanel         = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("KillingLogMiddle")));
    // MiddleKillerNameText        = Cast<UTextBlock>(GetWidgetFromName(TEXT("MidKillerName")));
    MiddleVictimInfoText        = Cast<UTextBlock>(GetWidgetFromName(TEXT("MidVictimInfo")));
    MiddleKillCountText         = Cast<UTextBlock>(GetWidgetFromName(TEXT("MidKillCount")));
    
    // Init Magnetic status instruction text
    MagneticFieldStatusInstructionText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MagneticFieldInstructionText")));
    
    if (!MagneticFieldStatusInstructionText)
        UC_Util::Print("From Instruction Widget NativeConstruct : MagneticFieldStatusInstructionText casting failed", FColor::Red, 10.f);
}

void UC_InstructionWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

    HandleConsumableInstructionFlicker(InDeltaTime);
    HandleLogFadeOut(InDeltaTime);

    HandleLogLifeTimers(InDeltaTime);
    
    // HandlePlayerWarningLogPositionsAndDefaultAlpha(InDeltaTime);
    HandleLogQueuePositionsAndDefaultAlpha(InDeltaTime, EQueueLogType::PlayerWarningLog);
    HandleLogQueuePositionsAndDefaultAlpha(InDeltaTime, EQueueLogType::TopKillFeedLog);
}

void UC_InstructionWidget::HandleConsumableInstructionFlicker(const float& DeltaTime)
{
    if (ConsumableInstruction->GetVisibility() == ESlateVisibility::Hidden) return;

    static float TimeAccumulator{};
    TimeAccumulator += DeltaTime;

    const float MinAlpha        = 0.1f;
    const float MaxAlpha        = 1.f;
    const float FlickerSpeed    = 5.f;

    float Alpha = MinAlpha + (MaxAlpha - MinAlpha) * (0.5f + 0.5f * FMath::Sin(FlickerSpeed * TimeAccumulator));
    ConsumableCurrentUsingTextBlock->SetRenderOpacity(Alpha);
}

void UC_InstructionWidget::HandleLogFadeOut(const float& DeltaTime)
{
    for (TSet<UWidget*>::TIterator It(FadeOutLogs); It; ++It)
    {
        UWidget* Log = *It;
        float CurrentAlpha = Log->GetRenderOpacity();

        if (CurrentAlpha < 0.05f) // Alpha가 충분히 작아서 Destination에 도달했다고 판단
        {
            Log->SetRenderOpacity(0.f);
            It.RemoveCurrent();
            continue;
        }

        // Lerp Alpha color to 0.f
        float DestAlpha = FMath::Lerp(CurrentAlpha, 0.f, DeltaTime * 10.f);
        Log->SetRenderOpacity(DestAlpha);
    }
}

void UC_InstructionWidget::HandleLogLifeTimers(const float& DeltaTime)
{
    for (TMap<UWidget*, float>::TIterator It(LogLifeTimers); It; ++It)
    {
        TPair<UWidget*, float>& TextBlockLifeTimePair = *It;
        TextBlockLifeTimePair.Value -= DeltaTime;

        if (TextBlockLifeTimePair.Value <= 0.f) // 수명이 다한 TextBlockLifeTimePair
        {
            StartFadeOut(TextBlockLifeTimePair.Key);
            It.RemoveCurrent();
        }
    }
}

void UC_InstructionWidget::HandleLogQueuePositionsAndDefaultAlpha(const float& DeltaTime, EQueueLogType QueueLogType)
{
    TArray<int>& LogSequence = (QueueLogType == EQueueLogType::PlayerWarningLog) ? PlayerWarningLogSequence : TopKillFeedLogSequence;
    TArray<FVector2D>& EachLogInitialPositions = (QueueLogType == EQueueLogType::PlayerWarningLog) ?
        PlayerWarningLogEachPositions : TopKillFeedLogEachPositions; 

    for (int i = 1; i < LogSequence.Num(); ++i)
    {
        int Index = LogSequence[i];
        UCanvasPanelSlot* CanvasPanelSlot = (QueueLogType == EQueueLogType::PlayerWarningLog) ?
            PlayerWarningLogTextPanels[Index] : TopKillFeedBlocks[Index].PanelSlot;

        FVector2D CurrentPosition = CanvasPanelSlot->GetPosition();
        FVector2D DestPosition = FMath::Lerp(CurrentPosition, EachLogInitialPositions[i], 20.f * DeltaTime);
        CanvasPanelSlot->SetPosition(DestPosition);
    }

    // 마지막에서 두 번째 -> 로그는 보이는데, FadeOut중이 아니라면 FadeOut 처리
    int LastPrevIndex = LogSequence[LogSequence.Num() - 2];
    UWidget* LastPrevLogWidget = (QueueLogType == EQueueLogType::PlayerWarningLog) ?
        Cast<UWidget>(PlayerWarningLogTexts[LastPrevIndex]) : TopKillFeedBlocks[LastPrevIndex].Panel;

    if (LastPrevLogWidget->GetRenderOpacity() > 0.f) // 아직 보이는 중이고
    {
        if (!FadeOutLogs.Contains(LastPrevLogWidget)) // Start Fade 처리가 안된 상황
        {
            LogLifeTimers.Remove(LastPrevLogWidget);
            StartFadeOut(LastPrevLogWidget);
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
    int LastPrevIndex = PlayerWarningLogSequence[2];
    UWidget* LastPrevLogWidget = PlayerWarningLogTexts[LastPrevIndex]; 
    if (LastPrevLogWidget->GetRenderOpacity() > 0.f) // 아직 보이는 중이고,
    {
        if (!FadeOutLogs.Contains(LastPrevLogWidget)) // Start Fade 처리가 안된 상황
        {
            LogLifeTimers.Remove(LastPrevLogWidget);
            StartFadeOut(LastPrevLogWidget);
        }
    }
}

void UC_InstructionWidget::HandleTopKillFeedBlockPositionsAndDefaultAlpha(const float& DeltaTime)
{
    for (int i = 1; i < TopKillFeedLogSequence.Num(); ++i)
    {
        int Index = TopKillFeedLogSequence[i];

        UCanvasPanelSlot* KillFeedBlockPanelSlot = TopKillFeedBlocks[Index].PanelSlot; 
        
        FVector2D CurrentPosition = KillFeedBlockPanelSlot->GetPosition();
        FVector2D DestPosition = FMath::Lerp(CurrentPosition, TopKillFeedLogEachPositions[i], 20.f * DeltaTime);
        KillFeedBlockPanelSlot->SetPosition(DestPosition);
    }

    int LastPrevIndex = TopKillFeedLogSequence[TopKillFeedLogSequence.Num() - 2];
    UWidget* KillFeedBlockPanel = TopKillFeedBlocks[LastPrevIndex].Panel;
    
    if (KillFeedBlockPanel->GetRenderOpacity() > 0.f) // 아직 보이는 중이고,
    {
        if (!FadeOutLogs.Contains(KillFeedBlockPanel)) // Start Fade 처리가 안된 상황
        {
            LogLifeTimers.Remove(PlayerWarningLogTexts[LastPrevIndex]);
            StartFadeOut(PlayerWarningLogTexts[LastPrevIndex]);
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
    ConsumableCurrentUsingTextBlock->SetRenderOpacity(1.f);
    return true;
}

bool UC_InstructionWidget::ActivateMagneticFieldInstructionText(FString InstructionString)
{
    // 내용 setting 하기
    MagneticFieldStatusInstructionText->SetText(FText::FromString(InstructionString));
    
    // Render Alpha값 초기 지정
    MagneticFieldStatusInstructionText->SetRenderOpacity(1.f);

    // Maximum 3초의 Log LifeTime 처리
    ApplyNewLifeTimerToLog(MagneticFieldStatusInstructionText, 3.f);
    
    return true;
}

bool UC_InstructionWidget::ActivateMiddleKillFeedLog(const FKillFeedDescriptor& KillFeedDescriptor)
{
    // 가운데 KillFeed와 관련 없는 정보가 들어왔을 때
    if (KillFeedDescriptor.DamageCauser != GAMESCENE_MANAGER->GetPlayer() &&
        KillFeedDescriptor.DamageTaker != GAMESCENE_MANAGER->GetPlayer())
        return false;

    MiddleKillFeedPanel->SetRenderOpacity(1.f);
    MiddleKillCountText->SetVisibility(ESlateVisibility::Hidden);
    ApplyNewLifeTimerToLog(MiddleKillFeedPanel, 3.f);

    // Player 자신이 죽었을 때
    if (KillFeedDescriptor.DamageTaker == GAMESCENE_MANAGER->GetPlayer())
    {
        // MiddleKillerNameText->SetText(FText::FromString("YOU"));
        
        switch (KillFeedDescriptor.DamageType)
        {
        case EDamageType::Weapon:
            
            if (KillFeedDescriptor.DamageCauser == GAMESCENE_MANAGER->GetPlayer())
            {
                FString Str = "yourself by " + KillFeedDescriptor.DamageCausedWeapon->GetKillLogWeaponName();
                MiddleVictimInfoText->SetText(FText::FromString(Str));
            }
            else // 다른 Character의 무기에 의해 Player가 죽었을 때
            {
                FString Str = "by " + KillFeedDescriptor.DamageCauser->GetCharacterName() +
                    " with " + KillFeedDescriptor.DamageCausedWeapon->GetKillLogWeaponName() +
                        " ("+ FString::FromInt(KillFeedDescriptor.Distance) + "m)";
                MiddleVictimInfoText->SetText(FText::FromString(Str));
            }
            
            return true;
        case EDamageType::BlueZone:
            MiddleVictimInfoText->SetText(FText::FromString("by BlueZone"));
            return true;
        case EDamageType::Drown:
            MiddleVictimInfoText->SetText(FText::FromString("yourself by Drowning"));
            return true;
        case EDamageType::Fall:
            MiddleVictimInfoText->SetText(FText::FromString("yourself by Falling"));
            return true;
        case EDamageType::VehicleHit:
        {
            FString Str = "by " + KillFeedDescriptor.DamageCauser->GetCharacterName() + "'s Vehicle";
            MiddleVictimInfoText->SetText(FText::FromString(Str));
        }
            return true;
        default: return false;
        }
    }
    
    MiddleKillCountText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    FString KillCountString = FString::FromInt(KillFeedDescriptor.DamageCauser->GetKillCount()) + " KILL";
    MiddleKillCountText->SetText(FText::FromString(KillCountString));
    
    // Player가 다른 Character를 죽였을 때
    if (KillFeedDescriptor.DamageType == EDamageType::VehicleHit)
    {
        FString Str = KillFeedDescriptor.DamageTaker->GetCharacterName();
        Str += " by Vehicle hit";
        MiddleVictimInfoText->SetText(FText::FromString(Str));
        return true;
    }
    
    FString Str = KillFeedDescriptor.DamageTaker->GetCharacterName();
    if (KillFeedDescriptor.bDamagedByHeadShot) Str += " by HeadShot";
    Str += " with " + KillFeedDescriptor.DamageCausedWeapon->GetKillLogWeaponName();
    Str += " (" + FString::FromInt(KillFeedDescriptor.Distance) + "m)";
    MiddleVictimInfoText->SetText(FText::FromString(Str));
    return true;
}

bool UC_InstructionWidget::AddPlayerWarningLog(const FString& WarningLog)
{
    // 새로운 첫 번째 로그로 맨 뒤 로그 이동시키기
    int TargetIndex = PlayerWarningLogSequence.Last();
    PlayerWarningLogSequence.RemoveAt(PlayerWarningLogSequence.Num() - 1);
    PlayerWarningLogSequence.Insert(TargetIndex, 0);
    /////////////////////////////////////////////////////////////////////
    
    UTextBlock* TargetTextBlock = PlayerWarningLogTexts[TargetIndex]; 

    // 내용 setting 하기
    TargetTextBlock->SetText(FText::FromString(WarningLog));

    // Render Alpha값 기본 값으로 지정
    TargetTextBlock->SetRenderOpacity(1.f);

    // 초기 위치 지정
    PlayerWarningLogTextPanels[TargetIndex]->SetPosition(PlayerWarningLogEachPositions[0]);
    
    FTimerHandle TimerHandle{};

    // Maximum 3초의 Log LifeTime 처리
    ApplyNewLifeTimerToLog(TargetTextBlock, 3.f);
    
    return true;
}

bool UC_InstructionWidget::AddTopKillFeedLog(const struct FKillFeedDescriptor& KillFeedDescriptor)
{
    int TargetIndex = TopKillFeedLogSequence.Last();
    TopKillFeedLogSequence.RemoveAt(TopKillFeedLogSequence.Num() - 1);
    TopKillFeedLogSequence.Insert(TargetIndex, 0);
    
    /////////////////////////////////////////////////////////////////////
    FTopKillFeedBlock& KillFeedBlock = TopKillFeedBlocks[TargetIndex];    
    
    // 내용 setting 하기
    /* Damage Causer */
    if (KillFeedDescriptor.DamageCauser)
    {
        KillFeedBlock.KillerNumberPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        KillFeedBlock.KillerText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

        // Set Killer Number
        FString Number = FString::FromInt(KillFeedDescriptor.DamageCauser->GetCharacterNumber());
        KillFeedBlock.KillerNumberText->SetText(FText::FromString(Number));

        // Set Killer Name
        KillFeedBlock.KillerText->SetText(FText::FromString(KillFeedDescriptor.DamageCauser->GetCharacterName()));
    }
    else
    {
        KillFeedBlock.KillerNumberPanel->SetVisibility(ESlateVisibility::Hidden);
        KillFeedBlock.KillerText->SetVisibility(ESlateVisibility::Hidden);
    }

    /* KillFeed Image */
    if (KillFeedDescriptor.DamageType != EDamageType::Weapon) // 무기가 아닌 다른 방식으로 사망했을 때
    {
        UTexture2D* KillLogTexture = KillLogTextures[KillFeedDescriptor.DamageType];
        if (!KillLogTexture) UC_Util::Print("From AddTopKillFeedLog : No KillLogTexture founded on current DamageType!", FColor::Red, 10.f);
        KillFeedBlock.KillFeedImage->SetBrushFromTexture(KillLogTexture);

        // Offset 조정
        float LeftRightOffset = DamageTypeKillLogImageLeftRightOffsets[KillFeedDescriptor.DamageType];
        KillFeedBlock.KillFeedImagePanelSlot->SetOffsets(FMargin(LeftRightOffset, 0.f));
    }
    else // 무기로 사망했을 때
    {
        AC_Weapon* DamageCausedWeapon = KillFeedDescriptor.DamageCausedWeapon; 
        UTexture2D* KillLogTexture = DamageCausedWeapon->GetKillLogTexture();
        if (!KillLogTexture) UC_Util::Print("From AddTopKillFeedLog : Weapon KillLogTexture nullptr!", FColor::Red, 10.f);
        KillFeedBlock.KillFeedImage->SetBrushFromTexture(KillLogTexture);

        // 수류탄 20.f, Pan 10.f, 나머지 0.f
        float LeftRightOffset = (Cast<AC_ThrowingWeapon>(DamageCausedWeapon)) ? 20.f :
                                (Cast<AC_MeleeWeapon>(DamageCausedWeapon))    ? 10.f : 0.f;
        // Offset 조정
        KillFeedBlock.KillFeedImagePanelSlot->SetOffsets(FMargin(LeftRightOffset, 0.f));
    }

    /* HeadShot Image */
    KillFeedBlock.HeadShotImage->SetVisibility(KillFeedDescriptor.bDamagedByHeadShot ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);

    /* Victim */
    if (KillFeedDescriptor.DamageTaker)
    {
        KillFeedBlock.VictimNumberText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        KillFeedBlock.VictimText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

        // Set Victim Number
        FString Number = FString::FromInt(KillFeedDescriptor.DamageTaker->GetCharacterNumber());
        KillFeedBlock.VictimNumberText->SetText(FText::FromString(Number));

        // Set Victim Name
        KillFeedBlock.VictimText->SetText(FText::FromString(KillFeedDescriptor.DamageTaker->GetCharacterName()));
    }
    else
    {
        UC_Util::Print("From AddTopKillFeedLog : DamageTaker not inited!", FColor::Red, 10.f);
        KillFeedBlock.VictimNumberText->SetVisibility(ESlateVisibility::Hidden);
        KillFeedBlock.VictimText->SetVisibility(ESlateVisibility::Hidden);
    }
    /////////////////////////////////////////////////////////////////////
    
    // Render Alpha값 기본 값으로 지정
    KillFeedBlock.Panel->SetRenderOpacity(1.f);

    FTimerHandle TimerHandle{};

    // Maximum 5초의 Log LifeTime 처리
    ApplyNewLifeTimerToLog(KillFeedBlock.Panel, 5.f);

    // 초기 위치 지정
    KillFeedBlock.PanelSlot->SetPosition(TopKillFeedLogEachPositions[0]);
    
    return true;
}

void UC_InstructionWidget::ApplyNewLifeTimerToLog(UWidget* Log, float TotalLifeTime)
{
    LogLifeTimers.Add(Log, TotalLifeTime);
    FadeOutLogs.Remove(Log);
}

