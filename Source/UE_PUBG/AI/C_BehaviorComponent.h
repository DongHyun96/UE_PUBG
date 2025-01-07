// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_BehaviorComponent.generated.h"

// TODO : Behavior type 카테고리 수정
UENUM(BlueprintType)
enum class EBehaviorType : uint8
{
	WAIT,
	PATROL,
	TRACE,
	ATTACK,
	HIT,
	DEATH
};


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

	void SetPlayer(class AActor* InPlayer);
	FName GetPlayerKey() const { return PlayerKey; }
	class AC_BasicCharacter* GetPlayer();


	void ChangeType(EBehaviorType Type);

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName BehaviorKey = "Behavior";

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName PlayerKey = "Player";


private:
	class UBlackboardComponent* Blackboard{};
};
