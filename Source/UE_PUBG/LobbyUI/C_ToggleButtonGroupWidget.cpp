// Fill out your copyright notice in the Description page of Project Settings.


#include "C_ToggleButtonGroupWidget.h"

#include <UIAutomationCore.h>

#include "C_LobbyWidget.h"
#include "C_ToggleButtonWidget.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/PanelWidget.h"
#include "Singleton/C_GameInstance.h"
#include "Utility/C_Util.h"

void UC_ToggleButtonGroupWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (GetRootWidget())
	{
		FindAllToggleButtonWidgets(GetRootWidget(), ToggleButtons);
		
		for (UC_ToggleButtonWidget* Button : ToggleButtons)
			Button->SetParentToggleButtonGroup(this);
	}

	for (int i = 0; i < ToggleButtons.Num(); ++i)
	{
		EHorizontalBoxPlace BoxPlace =	(i == 0) ? EHorizontalBoxPlace::FarLeft :
										(i != ToggleButtons.Num() - 1) ? EHorizontalBoxPlace::Middle : EHorizontalBoxPlace::FarRight;
		ToggleButtons[i]->SetHorizontalBoxPlace(BoxPlace);
	}

	// FindParentLobbyWidget();
}

void UC_ToggleButtonGroupWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UC_ToggleButtonGroupWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	HandleLerpButtonMargins(InDeltaTime);
}

void UC_ToggleButtonGroupWidget::Init(UC_LobbyWidget* InParentLobbyWidget)
{
	ParentLobbyWidget = InParentLobbyWidget;
	
	for (UC_ToggleButtonWidget* Button : ToggleButtons)
	{
		if (UC_GameInstance* GameInstance = Cast<UC_GameInstance>(GetGameInstance()))
			if (Button->GetLevelType() == GameInstance->GetCurrentSelectedLevelType())
			{
				Button->Select();
				break;
			}
	}
}

void UC_ToggleButtonGroupWidget::SetCurrentSelectedButton(UC_ToggleButtonWidget* InButton)
{
	UC_ToggleButtonWidget* PrevSelected = CurrentSelectedButton;
	CurrentSelectedButton = InButton;
	ParentLobbyWidget->OnSelectedMapChanged(InButton->GetLevelType());
	
	if (PrevSelected == CurrentSelectedButton || !PrevSelected) return;
	PrevSelected->OnUnSelected();
}

void UC_ToggleButtonGroupWidget::FindAllToggleButtonWidgets(UWidget* ParentWidget, TArray<UC_ToggleButtonWidget*>& OutArray)
{
	if (!ParentWidget) return;

	if (UC_ToggleButtonWidget* ToggleButton = Cast<UC_ToggleButtonWidget>(ParentWidget))
		OutArray.Add(ToggleButton);

	if (UPanelWidget* Panel = Cast<UPanelWidget>(ParentWidget))
	{
		const int32 ChildCount = Panel->GetChildrenCount();

		for (int32 i = 0; i < ChildCount; i++)
		{
			UWidget* Child = Panel->GetChildAt(i);
			FindAllToggleButtonWidgets(Child, OutArray);
		}
	}
}

void UC_ToggleButtonGroupWidget::HandleLerpButtonMargins(float DeltaTime)
{
	for (UC_ToggleButtonWidget* ToggleButton : ToggleButtons)
	{
		UHorizontalBoxSlot* HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(ToggleButton->Slot);

		FMargin SlotPaddingDest = (ToggleButton == CurrentSelectedButton) ? FMargin(0.f) : ToggleButton->GetIdleMargin();
		FMargin SlotPadding = LerpMargin(HorizontalBoxSlot->GetPadding(), SlotPaddingDest, DeltaTime * 15.f);
		HorizontalBoxSlot->SetPadding(SlotPadding);
	}
}

/*void UC_ToggleButtonGroupWidget::FindParentLobbyWidget()
{
	UObject* Current = this;

	while (Current)
	{
		if (UC_LobbyWidget* Found = Cast<UC_LobbyWidget>(Current))
		{
			ParentLobbyWidget = Found;
			return;
		}

		if (UWidget* WidgetParent = Cast<UWidget>(Current)) Current = WidgetParent->GetParent();
		else Current = Current->GetOuter();
	}
	
	if (!IsValid(ParentLobbyWidget))
		UC_Util::Print("ParentWidget init failed!", FColor::Red, 10.f);
}*/