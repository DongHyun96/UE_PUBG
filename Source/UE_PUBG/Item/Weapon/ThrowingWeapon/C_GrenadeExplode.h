// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "I_ExplodeStrategy.h"
#include "C_GrenadeExplode.generated.h"

UCLASS(Blueprintable)
class UE_PUBG_API AC_GrenadeExplode : public AActor, public II_ExplodeStrategy
{
	GENERATED_BODY()

public:
	AC_GrenadeExplode();
	
private:

	// II_ExplodeStrategy을(를) 통해 상속됨
	bool UseStrategy(AC_ThrowingWeapon* ThrowingWeapon) override;

private:

	// 각 피격 부위별 Damage rate
	TMap<FName, float> BodyPartsDamageRate{};

	// Ray를 쏠 Destination bone names
	TArray<FName> LineTraceDestBoneNames{};

	const float DAMAGE_BASE = 50.f;

};
