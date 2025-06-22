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

USTRUCT(BlueprintType)
struct FTutorialStageData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_TutorialStageChecker* TutorialStageChecker{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class ATriggerBox* StageStartTriggerBox{};

	
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

	static FString GetTutorialStageName(ETutorialStage Stage);

protected:

	UPROPERTY(BlueprintReadOnly)
	ETutorialStage CurrentStage{};
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<ETutorialStage, FTutorialStageData> TutorialStageInfos{};
};
