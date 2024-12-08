// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "C_SR.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_SR : public AC_Gun
{
	GENERATED_BODY()
public:
	AC_SR();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void ExecuteReloadMontage() override;
	FName SR_RELOAD_LEFT_HAND_SOCKET_NAME ="";
};
