// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Loot/C_BasicLoot.h"
#include "C_AirDropBox.generated.h"

UCLASS()
class UE_PUBG_API AC_AirDropBox : public AC_BasicLoot
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AC_AirDropBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
