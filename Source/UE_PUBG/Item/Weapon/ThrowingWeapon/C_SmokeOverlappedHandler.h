// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_SmokeOverlappedHandler.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FDele_SmokeDurationFinished, class UC_SmokeOverlappedHandler*);

/// <summary>
/// Smoke Grenade의 Actor Component, Smoke에 Overlapped 중인 Character 처리 담당
/// </summary>
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_PUBG_API UC_SmokeOverlappedHandler : public UActorComponent
{
	GENERATED_BODY()

public:
	UC_SmokeOverlappedHandler();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool SetOwnerSmokeGrenade(class AC_ThrowingWeapon* InOwnerSmokeGrenade);

	// OwnerSmokeGrenade Explode Start 될 때 호출될 함수
	void OnExplodeStart();

	/// <summary>
	/// Owner SmokeGrenade의 Destroy 직전 호출될 함수
	/// </summary>
	void OnDestroyOwnerSmokeGrenade();	

public:
	
	/// <summary>
	/// Smoke Area 안의 랜덤한 Location Getter 
	/// </summary>
	FVector GetRandomLocationInSmokeArea();

private:
	
	/// <summary>
	/// 시간에 따른 Smoke Area 크기 및 위치 조정
	/// </summary>
	void UpdateSmokeAreaCollider(const float& DeltaTime);

private:

	UFUNCTION()
	void OnSmokeAreaBeginOverlap
	(
		UPrimitiveComponent*	OverlappedComponent,
		AActor*					OtherActor,
		UPrimitiveComponent*	OtherComp,
		int32					OtherBodyIndex,
		bool					bFromSweep,
		const FHitResult&		SweepResult
	);
	
	UFUNCTION()
	void OnSmokeAreaEndOverlap
	(	UPrimitiveComponent*	OverlappedComponent,
		AActor*					OtherActor,
		UPrimitiveComponent*	OtherComp,
		int32					OtherBodyIndex
	);
	
private:

	AC_ThrowingWeapon* OwnerSmokeGrenade{};

public:
	
	FDele_SmokeDurationFinished Delegate_OnSmokeDurationFinished;

protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UBoxComponent* SmokeAreaCollider{};

private:

	bool  bHasExploded{};
	float Timer{};

	FVector SmokeAreaOriginLocation{};
};


