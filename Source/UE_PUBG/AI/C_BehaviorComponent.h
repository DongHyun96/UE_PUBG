// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "Service/C_BTServiceIdle.h"
// #include "Service/C_BTServiceIdle.h"
#include "C_BehaviorComponent.generated.h"

UENUM(BlueprintType)
enum class EServiceType : uint8
{
	IDLE,
	SKYDIVE,
	COMBAT,
	MAX
};

enum class EIdleTaskType   	: uint8;
enum class EPoseState		: uint8;

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
	EServiceType GetServiceType() const;

public:

	void SetOwnerEnemy(class AC_Enemy* InOwnerEnemy) { OwnerEnemy = InOwnerEnemy; }
	void SetOwnerEnemyAIController(class AC_EnemyAIController* InEnemyAIController) { OwnerEnemyAIController = InEnemyAIController; }

public:

	bool SetIdleTaskType(EIdleTaskType Type);
	EIdleTaskType GetIdleTaskType() const;

private:
	/// <summary>
	/// TargetCharacter가 죽었을 때 Callback을 받는 함수
	/// </summary>
	void OnTargetCharacterDead(class AC_BasicCharacter* DeadCharacter);
	
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName PlayerKey = "Player";

public:

	float GetWaitTime() const { return WaitTime; }
	
	/// <summary>
	/// TargetCharacter 세팅하기 
	/// </summary>
	/// <param name="InTargetCharacter"> : 공격 대상, 회피 대상 등이 될 TargetCharacter </param>
	/// <returns> nullptr나 valid하지 않은 Actor로 setting되었다면 return false </returns>
	bool SetTargetCharacter(class AC_BasicCharacter* InTargetCharacter);

	UFUNCTION(BlueprintCallable)
	class AC_BasicCharacter* GetTargetCharacter() const;

public:

	bool SetBasicTargetLocation(const FVector& InTargetLocation);
	FVector GetBasicTargetLocation() const;

	bool SetInCircleTargetLocation(const FVector& InTargetLocation);
	FVector GetInCircleTargetLocation() const;


private:

	AC_Enemy* OwnerEnemy{};
	class AC_EnemyAIController* OwnerEnemyAIController{};
	
protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName ServiceKey = "Service";

	// 현재 교전중인 Character
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName TargetCharacterKey = "TargetCharacter";

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName BasicTargetLocationKey = "BasicTargetLocation";

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName InCircleTargetLocationKey = "InCircleTargetLocation";
	
protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName IdleTaskKey = "IdleTask";

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName CombatTaskKey = "CombatTask";

private:
	class UBlackboardComponent* Blackboard{};

private: // WAIT_TASK 시간

	float WaitTime{};

private:
	
	// FlashBang Effect Duration
	float FlashBangEffectDuration{};
};



