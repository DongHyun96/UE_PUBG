// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/C_Weapon.h"
#include "C_MeleeWeapon.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_MeleeWeapon : public AC_Weapon
{
	GENERATED_BODY()

public:

	AC_MeleeWeapon();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};