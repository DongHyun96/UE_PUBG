// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Gun/C_Gun.h"
#include "C_AR.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_AR : public AC_Gun
{
	GENERATED_BODY()
	
public:
	AC_AR();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
