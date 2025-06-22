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

	class UC_TutorialStageChecker* TutorialStageChecker{};

	UPROPERTY(BlueprintReadOnly, EditInstanceOnly)
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

	TArray<UC_TutorialStageChecker*> TutorialStageCheckers{};

protected: // 리플렉션 문제 때문에 struct 안에서의 UObject 포인터 default 내용이 details에 나오지 않는 문제로 밑에 처럼 구조를 처리해야 함

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UC_TutorialStageChecker* MovementStageChecker{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UC_TutorialStageChecker* WeaponStageChecker{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UC_TutorialStageChecker* ThrowableStageChecker{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UC_TutorialStageChecker* HealingStageChecker{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UC_TutorialStageChecker* EndStageChecker{};
	
};
