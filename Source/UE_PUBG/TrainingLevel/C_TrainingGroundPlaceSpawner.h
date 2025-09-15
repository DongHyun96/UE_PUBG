// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_TrainingGroundPlaceSpawner.generated.h"

enum class ETrainingGroundPlace : uint8;

UCLASS()
class UE_PUBG_API AC_TrainingGroundPlaceSpawner : public AActor
{
	GENERATED_BODY()

public:
	AC_TrainingGroundPlaceSpawner();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:

	// 이 Spawner에 해당하는 Place위치 에디터에서 지정
	UPROPERTY(EditInstanceOnly)
	ETrainingGroundPlace PlaceName{};

private:

	
	
};
