// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_EnemyCombatFieldManager.generated.h"

// 관전 처리 Type
enum class ESpectatorType : uint8
{
	Player,	// Player 시점 (기본 시점)
	Enemy1,	// Enemy1 관전
	Enemy2,	// Enemy2 관전
	Free,	// Free 시점
	Max
};

/// <summary>
/// Prefix (++Type) overload
/// </summary>
inline ESpectatorType& operator++(ESpectatorType& Type)
{
	uint8 TypeToInt = static_cast<uint8>(Type);
	uint8 MaxType   = static_cast<uint8>(ESpectatorType::Max);

	Type = (TypeToInt >= MaxType - 1) ? static_cast<ESpectatorType>(0) :
										static_cast<ESpectatorType>(static_cast<uint8>(Type) + 1);   
	return Type;
}

/// <summary>
/// Prefix (--Type) overload
/// </summary>
inline ESpectatorType& operator--(ESpectatorType& Type)
{
	uint8 TypeToInt = static_cast<uint8>(Type);
	
	Type = (TypeToInt == 0) ?	static_cast<ESpectatorType>(static_cast<uint8>(ESpectatorType::Max) - 1) :
								static_cast<ESpectatorType>(static_cast<uint8>(Type) - 1);
	return Type;
}


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

	void SetIsPlaying(bool InIsPlaying) { bIsPlaying = InIsPlaying; }
	bool GetIsPlaying() const { return bIsPlaying; }

	void IncreaseSpectatorType();
	void DecreaseSpectatorType();

private:

	

private:

	AC_CombatFieldManager* OwnerCombatFieldManager{};

	ESpectatorType CurrentSpectatorType{};

private:

	bool bIsPlaying{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<APawn> FreeSpectatorPawnClass{};

private:

	APawn* FreeSpectatorPawn{};
	
};
