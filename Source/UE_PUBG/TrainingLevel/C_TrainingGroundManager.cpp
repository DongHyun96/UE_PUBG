// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TrainingGroundManager.h"

#include "Character/C_Player.h"
#include "Singleton/C_GameInstance.h"
#include "Singleton/C_GameSceneManager.h"

AC_TrainingGroundManager::AC_TrainingGroundManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_TrainingGroundManager::BeginPlay()
{
	Super::BeginPlay();
}

void AC_TrainingGroundManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_TrainingGroundManager::SetPlayerToDefaultTrainingGroundTransform()
{
	GAMESCENE_MANAGER->GetPlayer()->SetActorTransform(TrainingGroundDefaultPlayerTransform);
}

