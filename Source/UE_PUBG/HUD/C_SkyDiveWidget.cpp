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

	this->Visibility = ESlateVisibility::Hidden;
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
}
