// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Component/SkyDivingComponent/C_SkyDivingComponent.h"
#include "C_EnemySkyDivingComponent.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_EnemySkyDivingComponent : public UC_SkyDivingComponent
{
	GENERATED_BODY()

public:

	UC_EnemySkyDivingComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	void SetSkyDivingStateDestination(const FVector& InDestination) { SkyDivingStateDestination = InDestination; }
	void SetParachutingStateDestination(const FVector& InDestination) { ParachutingStateDestination = InDestination; }

	FVector GetParachutingStateDestination() const { return ParachutingStateDestination; }

private:

	/// <summary>
	/// SetSkyDivingState -> Set to SKYDIVING STATE 내부의 Template method
	/// </summary>
	/// <returns></returns>
	void SetStateToSkyDivingState() override;

	/// <summary>
	/// SetSkyDivingState -> Set to PARACHUTING STATE 내부의 Template method
	/// </summary>
	/// <returns></returns>
	void SetStateToParachutingState() override;

	/// <summary>
	/// SetSkyDivingState -> Set to LANDING STATE 내부의 Template method
	/// </summary>
	/// <returns></returns>
	void SetStateToLandingState() override;
	
private:

	FVector SkyDivingStateDestination{};	// SKYDIVING State인 상태에서의 목적지
	FVector ParachutingStateDestination{};	// PARACHUTING State인 상태에서의 목적지 (LandingTargetLocation)
	
};
