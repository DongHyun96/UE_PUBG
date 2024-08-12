// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "I_ExplodeStrategy.h"
#include "C_FlashBangExplode.generated.h"

UCLASS(Blueprintable)
class UE_PUBG_API AC_FlashBangExplode : public AActor, public II_ExplodeStrategy
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:

	bool UseStrategy(AC_ThrowingWeapon* ThrowingWeapon) override;

};
