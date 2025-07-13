// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "TutorialStageChecker/C_TutorialStageChecker.h"
#include "C_TutorialStageTriggerBox.generated.h"

UCLASS()
class UE_PUBG_API AC_TutorialStageTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

public:
	
	AC_TutorialStageTriggerBox();

protected:
	
	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;

	/// <summary>
	/// Start trigger box 활성화 / 비활성화
	/// </summary>
	/// <param name="InIsEnabled"></param>
	void ToggleTriggerBox(bool InIsEnabled);

	void SetDelegateParam(const TPair<uint8, int>& InDelegateParam) { DelegateParam = InDelegateParam; }

private:

	/*UFUNCTION()
	void OnTriggerBoxBeginOverlap
	(
		UPrimitiveComponent*	OverlappedComponent,
		AActor*					OtherActor,
		UPrimitiveComponent*	OtherComp,
		int32					OtherBodyIndex,
		bool					bFromSweep,
		const FHitResult&		SweepResult
	);*/

	UFUNCTION()
	void OnTriggerBoxBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

private:

	class UWidgetComponent* TriangleWidgetComp{};

public:

	FTutorialStageGoalCheckerDelegate AreaArrivedDelegate{};

private:

	// Delegate 처리용 Param
	TPair<uint8, int> DelegateParam{};
	
};
