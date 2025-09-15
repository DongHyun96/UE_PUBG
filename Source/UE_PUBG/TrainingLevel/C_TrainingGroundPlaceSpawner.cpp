// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TrainingGroundPlaceSpawner.h"

#include "C_TrainingGroundManager.h"
#include "Components/CapsuleComponent.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"


// Sets default values
AC_TrainingGroundPlaceSpawner::AC_TrainingGroundPlaceSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AC_TrainingGroundPlaceSpawner::BeginPlay()
{
	Super::BeginPlay();

	UCapsuleComponent* SpawnPlaceCapsuleComponent = Cast<UCapsuleComponent>(GetDefaultSubobjectByName(TEXT("SpawnPlace")));

	if (!SpawnPlaceCapsuleComponent)
	{
		UC_Util::Print("From AC_TrainingGroundPlaceSpawner::BeginPlay : SpawnPlaceCapsuleComponent not found!", FColor::Red, 10.f);
		return;
	}

	// Init Spawn Transform on TrainingGroundManager
	GAMESCENE_MANAGER->GetTrainingGroundManager()->AddTransformToTrainingGroundPlaceSpawnTransforms(PlaceName, SpawnPlaceCapsuleComponent->GetComponentTransform());
}

void AC_TrainingGroundPlaceSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

