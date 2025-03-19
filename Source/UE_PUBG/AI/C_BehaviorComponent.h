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

enum class EIdleTaskType   : uint8;
enum class ECombatTaskType : uint8;

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

	bool SetServiceType(EServiceType Type);

public:

	bool SetIdleTaskType(EIdleTaskType Type);
	bool SetCombatTaskType(ECombatTaskType Type);

public: // TODO : Player에 관련한 내용 지우기 (For Testing)

	bool SetPlayer(class AC_Player* Player);
	
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName PlayerKey = "Player";

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
	FName TargetCharacterKey = "TargetCharacter";

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName TargetLocationKey = "TargetLocation";
	
protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName IdleTaskKey = "IdleTask";

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName CombatTaskKey = "CombatTask";
	
private:
	class UBlackboardComponent* Blackboard{};
};




