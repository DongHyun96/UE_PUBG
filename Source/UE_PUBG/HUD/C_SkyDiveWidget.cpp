// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/C_SkyDiveWidget.h"

#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

#include "Materials/MaterialInstanceDynamic.h"

#include "Character/C_Player.h"
#include "Character/Component/C_SkyDivingComponent.h"

#include "Utility/C_Util.h"

void UC_SkyDiveWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AltitudeBoxPanel) AltitudeBoxPanelSlot = Cast<UCanvasPanelSlot>(AltitudeBoxPanel->Slot);

	this->SetVisibility(ESlateVisibility::Hidden);

	// Init Speed Texts
	for (int i = 0; i < 7; i++)
	{
		FString WidgetName    = FString::Printf(TEXT("ST%d"), i);
		UTextBlock* TextBlock = Cast<UTextBlock>(GetWidgetFromName(*WidgetName));

		if (!TextBlock)
		{
			UC_Util::Print("No Text Block!", FColor::Red, 10.f);
			continue;
		}

		SpeedTextBlocks.Add(TextBlock);

		SpeedTextBlockSlots.Add(Cast<UCanvasPanelSlot>(TextBlock->Slot));
	}
}

void UC_SkyDiveWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateSpeed();
}

void UC_SkyDiveWidget::SetAltitude(const float& Altitude)
{
	if (!AltitudeTextBlock) return;

	// Setting text
	int AltitudeToMeter = Altitude * 0.01f;

	FString AltitudeString = FString::FromInt(AltitudeToMeter);

	AltitudeTextBlock->SetText(FText::FromString(AltitudeString));


	// Setting Altitude box position
	FVector2D PlayerPositionRange = (Altitude > ParachuteLimitAltitude) ? 
		                            FVector2D(JumpedAltitude, ParachuteLimitAltitude) : FVector2D(ParachuteLimitAltitude, 0.f);

	FVector2D PanelPositionRange = (Altitude > ParachuteLimitAltitude) ?
		                            ALTITUDE_BOX_MAX_TO_PARACHUTE : ALTITUDE_BOX_PARACHUTE_TO_MIN;

	float PositionY = FMath::GetMappedRangeValueClamped(PlayerPositionRange, PanelPositionRange, Altitude);
	float X			= AltitudeBoxPanelSlot->GetPosition().X;
	AltitudeBoxPanelSlot->SetPosition(FVector2D(X, PositionY));
}

void UC_SkyDiveWidget::UpdateSpeed()
{
	float Speed			= OwnerPlayer->GetVelocity().Size();
	int KiloPerHour		= Speed * 0.036f;
	float MatSpeedValue = (KiloPerHour) / 500.f; // 중간 값을 0으로 둠

	// ScaleBar 위치 맞추기
	if (SpeedScaleBarMatInstDynamic) SpeedScaleBarMatInstDynamic->SetScalarParameterValue("speed", MatSpeedValue);

	// SpeedText 띄우기
	CurrentSpeedTextBlock->SetText(FText::FromString(FString::FromInt(KiloPerHour)));

	// SpeedTextBlocks Update
	int SpeedPadding	= KiloPerHour % 50;
	int FirstSpeed		= KiloPerHour - 150 - SpeedPadding;
	int MidSpeedMargin	= KiloPerHour - FirstSpeed - 150;
	float MidMargin		= MidSpeedMargin * TEXTBLOCKSCALING_FACTOR;
	float X				= SpeedTextBlockSlots[0]->GetPosition().X;
	float FirstY		= SPEEDTEXTBLOCKS_Y_RANGE.X + MidMargin;

	for (int i = 0; i < SpeedTextBlocks.Num(); i++)
	{
		// Set text content
		int Current		= FirstSpeed + 50 * i;
		FText Content	= FText::FromString(FString::FromInt(Current));
		SpeedTextBlocks[i]->SetText(Content);

		// Set Y Position
		float Y = FirstY + SPEEDTEXTBLOCKS_STEP * i;
		SpeedTextBlockSlots[i]->SetPosition(FVector2D(X, Y));

		// Set Alpha
		const FVector2D MidToEndRange	= FVector2D(0.f, 201.f);
		float FromMidDiff				= FMath::Abs(Y - SPEEDTEXT_MID_Y_POS);
		float Alpha						= FMath::GetMappedRangeValueClamped(MidToEndRange, FVector2D(1, 0), FromMidDiff);

		FLinearColor TextBlockColor = SpeedTextBlocks[i]->ColorAndOpacity.GetSpecifiedColor();
		TextBlockColor.A = (Current < 0 || FromMidDiff > 201.f) ? 0.f : Alpha;
		SpeedTextBlocks[i]->SetColorAndOpacity(TextBlockColor);
	}
}
