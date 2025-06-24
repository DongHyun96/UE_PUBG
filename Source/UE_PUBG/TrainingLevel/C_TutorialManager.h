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

inline ETutorialStage& operator++(ETutorialStage& Type)
{
	uint8 TypeToInt = static_cast<uint8>(Type);
	uint8 MaxType   = static_cast<uint8>(ETutorialStage::Max);

	Type = (TypeToInt >= MaxType - 1) ? static_cast<ETutorialStage>(0) :
										static_cast<ETutorialStage>(static_cast<uint8>(Type) + 1);   
	return Type;
}

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

public:
	
	/// <summary>
	/// 다음 Stage로 넘어가기 처리 
	/// </summary>
	void SetStageToNextStage();
	
protected:

	UPROPERTY(BlueprintReadOnly)
	ETutorialStage CurrentStage{};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<ETutorialStage, class UC_TutorialStageChecker*> TutorialStageCheckers{};

	UPROPERTY(EditInstanceOnly)
	TMap<ETutorialStage, class ATriggerBox*> StageStartTriggerBoxes{};
	
};
