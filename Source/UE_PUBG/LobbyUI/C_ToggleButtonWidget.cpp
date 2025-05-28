// Fill out your copyright notice in the Description page of Project Settings.


#include "C_ToggleButtonWidget.h"

#include "C_ToggleButtonGroupWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Singleton/C_GameInstance.h"

const TMap<bool, FMargin> UC_ToggleButtonWidget::HoverMapNameMargins =
{
	{false, FMargin(10.f, 0.f, 0.f, 0.f)},
	{true,  FMargin(25.f, 0.f, 0.f, 0.f)},
}; // false(not hovered), true Hovered Left offset

const TMap<EHorizontalBoxPlace, FMargin> UC_ToggleButtonWidget::HorizontalBoxPlaceMargins =
{
	{EHorizontalBoxPlace::FarLeft,	FMargin(15.f, 15.f, 7.5f, 15.f)},
	{EHorizontalBoxPlace::Middle,	FMargin(7.5f, 15.f, 7.5f, 15.f)},
	{EHorizontalBoxPlace::FarRight, FMargin(7.5f, 15.f, 15.f, 15.f)}
};

void UC_ToggleButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	MapImage->SetBrushFromTexture(MapImageTexture);
	switch (LevelType)
	{
	case ELevelType::QuickStart: MapName->SetText(FText::FromString("QUICK START"));
		break;
	case ELevelType::ShantyTown: MapName->SetText(FText::FromString("SHANTY TOWN"));
		break;
	case ELevelType::TrainingGround: MapName->SetText(FText::FromString("TRAINING GROUND"));
		break;
	}
	
}

void UC_ToggleButtonWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const FVector2D MapImageScale = FMath::Lerp(MapImage->GetRenderTransform().Scale, MapImageScaleDest, 15.f * InDeltaTime);
	MapImage->SetRenderScale(MapImageScale);
}

void UC_ToggleButtonWidget::Select()
{
	if (ToggleState == EToggleState::Selected) return;

	ParentToggleButtonGroup->SetCurrentSelectedButton(this);
	/*if (UC_GameInstance* GameInstance = Cast<UC_GameInstance>(GetGameInstance()))
		GameInstance->SetCurrentSelectedLevelType(LevelType);*/\
	
	ToggleState = EToggleState::Selected;
	
	// 초기화 할 Element들 초기화
	MapImage->SetRenderScale(FVector2D(1.f));
	UCanvasPanelSlot* MapNameSlot = Cast<UCanvasPanelSlot>(MapName->Slot);
	MapNameSlot->SetOffsets(HoverMapNameMargins[false]);

	// Selected effect 구현
	BarImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	MapNameBGBlur->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UC_ToggleButtonWidget::OnUnSelected()
{
	ToggleState = EToggleState::Idle;
	
	MapImageScaleDest = FVector2D(1.f);
	BarImage->SetVisibility(ESlateVisibility::Hidden);
	MapNameBGBlur->SetVisibility(ESlateVisibility::Hidden);
}

void UC_ToggleButtonWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	if (ToggleState == EToggleState::Selected) return;

	// Hover effect 처리
	MapImageScaleDest = FVector2D(1.25f);

	UCanvasPanelSlot* MapNameSlot = Cast<UCanvasPanelSlot>(MapName->Slot);
	MapNameSlot->SetOffsets(HoverMapNameMargins[true]);
}

void UC_ToggleButtonWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	if (ToggleState == EToggleState::Selected) return;

	// Hover end effect 처리
	MapImageScaleDest = FVector2D(1.f);

	UCanvasPanelSlot* MapNameSlot = Cast<UCanvasPanelSlot>(MapName->Slot);
	MapNameSlot->SetOffsets(HoverMapNameMargins[false]);
}

FReply UC_ToggleButtonWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Select();
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}