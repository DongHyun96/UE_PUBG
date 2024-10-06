// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_SwimmingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_SwimmingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UC_SwimmingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter);

private:

	UFUNCTION()
	void OnWaterDetectionColliderBeginOverlap
	(
		UPrimitiveComponent*	OverlappedComponent,
		AActor*					OtherActor,
		UPrimitiveComponent*	OtherComp,
		int32					OtherBodyIndex,
		bool					bFromSweep,
		const FHitResult&		SweepResult
	);

	UFUNCTION()
	void OnWaterDetectionColliderEndOverlap
	(
		UPrimitiveComponent*	OverlappedComponent,
		AActor*					OtherActor,
		UPrimitiveComponent*	OtherComp,
		int32					OtherBodyIndex
	);

private:

	class AC_BasicCharacter* OwnerCharacter{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UCapsuleComponent* WaterDetectionCollider{};

private:

	bool bIsSwimming{};
		
};
