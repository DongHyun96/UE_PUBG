// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_SwimmingComponent.generated.h"


UENUM(BlueprintType)
enum class ESwimmingState : uint8
{
	ON_GROUND,
	SWIMMING_SURFACE,
	SWIMMING_UNDER,
	MAX
};

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

	void HandlePlayerMovement(const FVector2D& MovementVector);

	/// <summary>
	/// Swimming중 C 키 Input 처리
	/// </summary>
	void OnSwimmingCKey();

	/// <summary>
	/// Swimming 중 Spacebar 키 Input 처리
	/// </summary>
	void OnSwimmingSpaceBarKey();

	void OnSwimmingMoveEnd();

public:

	void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter);

	ESwimmingState GetSwimmingState() const { return SwimmingState; }

	bool IsSwimming() const { return SwimmingState != ESwimmingState::ON_GROUND; }

private:

	void HandleSwimmingState();

	/// <summary>
	/// OxygenAmount Handling 하기
	/// </summary>
	/// <param name="DeltaTime"></param>
	void UpdateOxygenAmount(const float& DeltaTime);

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

	void StartSwimming();
	void StopSwimming();

	/// <summary>
	/// 해당 위치의 수심 구하기
	/// </summary>
	/// <param name="Position"> : 수심을 구할 위치 </param>
	/// <returns> : 수심 </returns>
	float GetWaterDepth(const FVector& Position);

	/// <summary>
	/// WaterDetectionCollider로부터 물 바닥면까지의 깊이 구하기
	/// </summary>
	/// <returns> : 깊이 </returns>
	float GetCharacterDepth();

private:

	class AC_BasicCharacter*	OwnerCharacter{};
	class AC_Player*			OwnerPlayer{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UCapsuleComponent* WaterDetectionCollider{};

private:

	ESwimmingState SwimmingState{};

	// 현재 헤엄치고 있는 수면의 z값
	float EnteredWaterZ{};

private:

	const float CAN_WALK_DEPTH_LIMIT = 100.f;

private:

	bool bIsHandlingPlayerMovementOnCurrentTick{};
	
};
