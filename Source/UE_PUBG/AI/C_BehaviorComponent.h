// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_BehaviorComponent.generated.h"

UENUM(BlueprintType)
enum class EServiceType : uint8
{
	IDLE,
	SKYDIVE,
	COMBAT,
	FARMING,
	STAT_CARE,
	MAX
};

// TODO : Behavior type 카테고리 수정 (기존 수업 Behavior Type -> BT Service에서 해당 type transition 시켰음)
UENUM(BlueprintType)
enum class EBehaviorType : uint8
{
	WAIT,
	PATROL,
	TRACE,
	ATTACK,
	HIT,
	DEATH,
	MAX
};

enum class EIdleTaskType : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_BehaviorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UC_BehaviorComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetBlackboard(class UBlackboardComponent* InBlackboard) { Blackboard = InBlackboard; }


	bool SetBehaviorType(EBehaviorType Type);

	bool SetServiceType(EServiceType Type);

	bool SetIdleTaskType(EIdleTaskType Type);

public:
	
	/// <summary>
	/// TargetCharacter 세팅하기 
	/// </summary>
	/// <param name="InTargetCharacter"> : 공격 대상, 회피 대상 등이 될 TargetCharacter </param>
	/// <returns> 제대로 setting되었다면 return true </returns>
	bool SetTargetCharacter(AActor* InTargetCharacter);
	class AC_BasicCharacter* GetTargetCharacter() const;

	bool SetTargetLocation(const FVector& InTargetLocation);
	FVector GetTargetLocation() const;

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName ServiceKey = "Service";

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName BehaviorKey = "Behavior";
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName TargetCharacterKey = "TargetCharacter";

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName TargetLocationKey = "TargetLocation";
	
protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName IdleTaskKey = "IdleTask";

private:
	class UBlackboardComponent* Blackboard{};
};


