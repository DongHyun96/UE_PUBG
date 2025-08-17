// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_PlayerCombatFieldManager.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_PUBG_API UC_PlayerCombatFieldManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UC_PlayerCombatFieldManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	void SetOwnerCombatFieldManager(class AC_CombatFieldManager* InCombatFieldManager) { OwnerCombatFieldManager = InCombatFieldManager; }

private:

	AC_CombatFieldManager* OwnerCombatFieldManager{};
	
};
