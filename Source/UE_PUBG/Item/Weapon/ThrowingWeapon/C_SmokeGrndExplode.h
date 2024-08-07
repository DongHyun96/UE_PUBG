// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "I_ExplodeStrategy.h"

#include "C_SmokeGrndExplode.generated.h"

UCLASS(Blueprintable)
class UE_PUBG_API AC_SmokeGrndExplode : public AActor, public II_ExplodeStrategy
{
	GENERATED_BODY()
	
private:

	bool UseStrategy(AC_ThrowingWeapon* ThrowingWeapon) override;

};
