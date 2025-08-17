// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_EnemyCombatFieldManager.generated.h"

/// <summary>
/// Enemy vs Enemy CombatField 관전 카메라 처리 Managing
/// </summary>
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_PUBG_API UC_EnemyCombatFieldManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UC_EnemyCombatFieldManager();

protected:
	virtual void BeginPlay() override;

public:
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	void SetOwnerCombatFieldManager(class AC_CombatFieldManager* InCombatFieldManager) { OwnerCombatFieldManager = InCombatFieldManager; }

private:

	UFUNCTION()
	void OnSpectatorBoxBeginOverlap
	(
		UPrimitiveComponent*	OverlappedComponent,
		AActor*					OtherActor,
		UPrimitiveComponent*	OtherComp,
		int32					OtherBodyIndex,
		bool					bFromSweep,
		const FHitResult&		SweepResult
	);

	UFUNCTION()
	void OnSpectatorBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
private:

	AC_CombatFieldManager* OwnerCombatFieldManager{};

	// Enemy vs Enemy 관전 위치 (관전 카메라 처리할 수 있는 위치의 Box Component)
	TArray<class UBoxComponent*> SpectatorBoxes{};

};
