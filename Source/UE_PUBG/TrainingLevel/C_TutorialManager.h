// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_TutorialManager.generated.h"

UENUM(BlueprintType)
enum class ETutorialStage : uint8
{
	MovementTutorial,
	WeaponTutorial,
	ThrowableTutorial,
	HealingTutorial,
	TutorialEnd,
	Max
};

UCLASS()
class UE_PUBG_API AC_TutorialManager : public AActor
{
	GENERATED_BODY()

public:
	AC_TutorialManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(BlueprintReadOnly)
	ETutorialStage CurrentStage{};

	UPROPERTY(EditDefaultsOnly)
	TMap<ETutorialStage, class UC_TutorialStageChecker*> TutorialStageCheckers{};

	UPROPERTY(EditInstanceOnly)
	TMap<ETutorialStage, class ATriggerBox*> StageStartTriggerBoxes{};
	
};
