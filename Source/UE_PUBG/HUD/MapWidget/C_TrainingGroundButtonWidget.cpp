// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TrainingGroundButtonWidget.h"

#include "Chaos/ChaosPerfTest.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Singleton/C_GameSceneManager.h"
#include "TrainingLevel/C_TrainingGroundManager.h"
#include "TrainingLevel/Tutorial/C_TutorialManager.h"
#include "Utility/C_Util.h"

void UC_TrainingGroundButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	for (int i = 0; i < TrainingGroundPlaceButtonPanel->GetChildrenCount(); ++i)
	{
		UWidget* ChildWidget = TrainingGroundPlaceButtonPanel->GetChildAt(i);
		UButton* PlaceButton = Cast<UButton>(ChildWidget);
		if (!PlaceButton) continue;
		
		Buttons.Add(PlaceButton);
	}

	Buttons[0]->OnClicked.AddDynamic(this, &UC_TrainingGroundButtonWidget::OnShootingRangeButtonClicked);
	Buttons[1]->OnClicked.AddDynamic(this, &UC_TrainingGroundButtonWidget::OnSkyDivingFieldButtonClicked);
	Buttons[2]->OnClicked.AddDynamic(this, &UC_TrainingGroundButtonWidget::OnGrenadePitButtonClicked);
	Buttons[3]->OnClicked.AddDynamic(this, &UC_TrainingGroundButtonWidget::OnTutorialFieldButtonClicked);
	Buttons[4]->OnClicked.AddDynamic(this, &UC_TrainingGroundButtonWidget::OnSupplyDepotButtonClicked);
	Buttons[5]->OnClicked.AddDynamic(this, &UC_TrainingGroundButtonWidget::OnGarageButtonClicked);
	Buttons[6]->OnClicked.AddDynamic(this, &UC_TrainingGroundButtonWidget::OnAIMovementTestFieldButtonClicked);
	Buttons[7]->OnClicked.AddDynamic(this, &UC_TrainingGroundButtonWidget::OnAIStatCareTestFieldButtonClicked);
	Buttons[8]->OnClicked.AddDynamic(this, &UC_TrainingGroundButtonWidget::OnEVETestFieldButtonClicked);
	Buttons[9]->OnClicked.AddDynamic(this, &UC_TrainingGroundButtonWidget::OnPVETestFieldButtonClicked);
}

void UC_TrainingGroundButtonWidget::ToggleButtonsEnabled(bool bEnabled)
{
	for (UButton* Button : Buttons)
		Button->SetIsEnabled(bEnabled);
}

void UC_TrainingGroundButtonWidget::OnShootingRangeButtonClicked()
{
	GAMESCENE_MANAGER->GetTrainingGroundManager()->MovePlayerToPlaceSpawnTransform(ETrainingGroundPlace::ShootingRange);
}

void UC_TrainingGroundButtonWidget::OnSkyDivingFieldButtonClicked()
{
	GAMESCENE_MANAGER->GetTrainingGroundManager()->MovePlayerToPlaceSpawnTransform(ETrainingGroundPlace::SkyDivingField);
}

void UC_TrainingGroundButtonWidget::OnGrenadePitButtonClicked()
{
	GAMESCENE_MANAGER->GetTrainingGroundManager()->MovePlayerToPlaceSpawnTransform(ETrainingGroundPlace::GrenadePit);
}

void UC_TrainingGroundButtonWidget::OnTutorialFieldButtonClicked()
{
	// StartTutorial 처리에 Spawn 위치로 처리됨
	// GAMESCENE_MANAGER->GetTrainingGroundManager()->MovePlayerToPlaceSpawnTransform(ETrainingGroundPlace::TutorialField);
	GAMESCENE_MANAGER->GetTrainingGroundManager()->GetTutorialManager()->StartTutorial();
}

void UC_TrainingGroundButtonWidget::OnSupplyDepotButtonClicked()
{
	GAMESCENE_MANAGER->GetTrainingGroundManager()->MovePlayerToPlaceSpawnTransform(ETrainingGroundPlace::SupplyDepot);
}

void UC_TrainingGroundButtonWidget::OnGarageButtonClicked()
{
	GAMESCENE_MANAGER->GetTrainingGroundManager()->MovePlayerToPlaceSpawnTransform(ETrainingGroundPlace::Garage);
}

void UC_TrainingGroundButtonWidget::OnAIMovementTestFieldButtonClicked()
{
	GAMESCENE_MANAGER->GetTrainingGroundManager()->MovePlayerToPlaceSpawnTransform(ETrainingGroundPlace::AIMovementTestField);
}

void UC_TrainingGroundButtonWidget::OnAIStatCareTestFieldButtonClicked()
{
	GAMESCENE_MANAGER->GetTrainingGroundManager()->MovePlayerToPlaceSpawnTransform(ETrainingGroundPlace::AIStatCareTestField);
}

void UC_TrainingGroundButtonWidget::OnEVETestFieldButtonClicked()
{
	GAMESCENE_MANAGER->GetTrainingGroundManager()->MovePlayerToPlaceSpawnTransform(ETrainingGroundPlace::EVETestField);
}

void UC_TrainingGroundButtonWidget::OnPVETestFieldButtonClicked()
{
	GAMESCENE_MANAGER->GetTrainingGroundManager()->MovePlayerToPlaceSpawnTransform(ETrainingGroundPlace::PVETestField);

}
