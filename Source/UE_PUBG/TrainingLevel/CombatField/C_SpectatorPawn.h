// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "C_SpectatorPawn.generated.h"

UCLASS()
class UE_PUBG_API AC_SpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()

public:
	AC_SpectatorPawn();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	
	virtual void MoveForward(float Val) override;
	virtual void MoveRight(float Val) override;

	virtual void TurnAtRate(float Rate) override;

	
};
