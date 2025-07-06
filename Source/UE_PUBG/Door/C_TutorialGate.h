// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_TutorialGate.generated.h"

UCLASS()
class UE_PUBG_API AC_TutorialGate : public AActor
{
	GENERATED_BODY()

public:
	AC_TutorialGate();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void CloseGate();

	/// <summary>
	/// Opening Trigger Box 활성화 / 비활성화
	/// </summary>
	void ToggleOpeningBoxTriggerEnabled(bool Enabled);

	UFUNCTION(BlueprintImplementableEvent)
	void OpenGate();

private:

	UFUNCTION()
	void OnGateOpeningBoxBeginOverlap
	(
		UPrimitiveComponent*	OverlappedComp,
		AActor*					OtherActor,
		UPrimitiveComponent*	OtherComp,
		int32					OtherBodyIndex,
		bool					bFromSweep,
		const FHitResult&		SweepResult
	);


private:

	UShapeComponent* GateOpeningBox{};
	
	class UWidgetComponent* TriangleWidgetComponent{}; // Goal 달성에 따라 Spawn dnld ddddd
	
};
