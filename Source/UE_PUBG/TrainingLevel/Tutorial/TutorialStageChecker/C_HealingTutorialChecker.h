// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "C_TutorialStageChecker.h"
#include "C_HealingTutorialChecker.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_PUBG_API UC_HealingTutorialChecker : public UC_TutorialStageChecker
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UC_HealingTutorialChecker();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
