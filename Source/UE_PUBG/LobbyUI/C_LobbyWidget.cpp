// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyUI/C_LobbyWidget.h"

#include "C_ToggleButtonGroupWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Singleton/C_GameInstance.h"
#include "Utility/C_Util.h"

void UC_LobbyWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	MapSelectToggleButtonGroup->Init(this);
}

void UC_LobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UC_LobbyWidget::OnSelectedMapChanged(ELevelType InSelected)
{
	PlayAnimation(BackgroundMapImageAnimation);
	PlayAnimation(MapNameSizeAnimation);
	MapSelectionMapImage->SetBrushFromTexture(LevelSelectionBackGroundImages[InSelected]);
	
	switch (InSelected)
	{
	case ELevelType::QuickStart:
		MapSelectedNameText->SetText(FText::FromString("QUICK START"));
		MapSelectedSizeText->SetText(FText::FromString("?X? MAP"));
		MapSelectedSizeText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	case ELevelType::ShantyTown:
		MapSelectedNameText->SetText(FText::FromString("SHANTY TOWN"));
		MapSelectedSizeText->SetText(FText::FromString("1X1 MAP"));
		MapSelectedSizeText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	case ELevelType::TrainingGround:
		MapSelectedNameText->SetText(FText::FromString("TRAINING GROUND"));
		MapSelectedSizeText->SetVisibility(ESlateVisibility::Hidden);
		break;
	}
}
