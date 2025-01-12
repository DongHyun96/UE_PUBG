// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "C_EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_EnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AC_EnemyAIController();

	void BeginPlay()				override;
	void Tick(float DeltaTime)		override;

	void OnPossess(APawn* InPawn)	override;

	/// <summary>
	/// Sight Radius안에 들어오면 발동할 함수
	/// </summary>
	/// <param name="UpdatedActors"> : 인지한 Actors 배열</param>
	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	class UC_BehaviorComponent* GetBehaviorComponent() const { return BehaviorComponent; }

protected:

	class UAISenseConfig_Sight* Sight{};

	class AC_Enemy* OwnerCharacter{};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float BehaviorRange = 150.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UC_BehaviorComponent* BehaviorComponent{};
};
