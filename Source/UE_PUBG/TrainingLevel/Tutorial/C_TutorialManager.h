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

// 각 Stage별 TriggerBoxes Holder 구조체
USTRUCT(BlueprintType)
struct FTutorialStageTriggerBoxes
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly)
	TArray<class AC_TutorialStageTriggerBox*> TriggerBoxes{};

	TArray<AC_TutorialStageTriggerBox*>& GetTriggerBoxesReference() { return TriggerBoxes; }
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

	/// <summary>
	/// Tutorial 시작하기
	/// </summary>
	void StartTutorial();

	/// <summary>
	/// 다음 Stage로 넘어가기 처리 
	/// </summary>
	void SetStageToNextStage();

	ETutorialStage GetCurrentStage() const { return CurrentStage; }

	/// <summary>
	/// 현재 Stage 초기화 & 목표 행동 Delegate 구독 처리
	/// </summary>
	void InitCurrentStageChecker();

public:

	class UC_TutorialWidget* GetTutorialWidget() const { return TutorialWidget; }

	TArray<AC_TutorialStageTriggerBox*>& GetTriggerBoxesReference(ETutorialStage TutorialStage)
	{ return TutorialStageTriggerBoxes[TutorialStage].GetTriggerBoxesReference(); }

protected:

	UPROPERTY(EditAnywhere)
	ETutorialStage CurrentStage{};
	
	UPROPERTY(EditInstanceOnly)
	TMap<ETutorialStage, class AC_TutorialStageTriggerBox*> StageStartTriggerBoxes{};

	// 다음 Stage로 나아갈 Gate 저장
	UPROPERTY(EditInstanceOnly)
	TMap<ETutorialStage, class AC_TutorialGate*> TutorialGates{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UC_TutorialWidget* TutorialWidget{};

	UPROPERTY(EditInstanceOnly)
	FTransform PlayerTutorialStartTransform{};
	
private:
	
	TMap<ETutorialStage, class UC_TutorialStageChecker*> TutorialStageCheckers{};

protected:

	// 각 Stage별 Trigger Box들
	UPROPERTY(EditInstanceOnly)
	TMap<ETutorialStage, FTutorialStageTriggerBoxes> TutorialStageTriggerBoxes{};
};
