// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TrainingGroundManager.h"

#include "C_AISkyDiveTesterManager.h"
#include "Character/C_Player.h"
#include "Singleton/C_GameInstance.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"

AC_TrainingGroundManager::AC_TrainingGroundManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_TrainingGroundManager::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(AISkyDiveTesterManager))
		UC_Util::Print("From AC_TrainingGroundManager::BeginPlay : AISkyDiveTesterManager not valid!", FColor::Red, 10.f);
}

void AC_TrainingGroundManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_TrainingGroundManager::SetPlayerToDefaultTrainingGroundTransform()
{
	GAMESCENE_MANAGER->GetPlayer()->SetActorTransform(TrainingGroundDefaultPlayerTransform);
}

bool AC_TrainingGroundManager::AddTransformToTrainingGroundPlaceSpawnTransforms(ETrainingGroundPlace TargetPlace, const FTransform& Transform)
{
	if (TargetPlace == ETrainingGroundPlace::Max) return false;

	if (TrainingGroundPlaceSpawnTransforms.Contains(TargetPlace))
	{
		UC_Util::Print
		(
			"From AC_TrainingGroundManager::AddTransformToTrainingGroundPlaceSpawnTransforms : TMap already contains Target place type transform!",
			FColor::Red, 10.f
		);
		
		return false;
	}
	TrainingGroundPlaceSpawnTransforms.Add(TargetPlace, Transform);
	return true;
}

bool AC_TrainingGroundManager::MovePlayerToPlaceSpawnTransform(ETrainingGroundPlace TargetPlace)
{
	if (TargetPlace == ETrainingGroundPlace::Max) return false;
	
	GAMESCENE_MANAGER->GetPlayer()->SetActorTransform(TrainingGroundPlaceSpawnTransforms[TargetPlace]);
	return true;
}

