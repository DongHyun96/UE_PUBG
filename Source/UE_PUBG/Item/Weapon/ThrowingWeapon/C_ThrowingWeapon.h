// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Weapon/C_Weapon.h"
#include "C_ThrowingWeapon.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_ThrowingWeapon : public AC_Weapon
{
	GENERATED_BODY()

public:

	AC_ThrowingWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	/// <summary>
	/// Throwable�� Holster�� ���� ���� ���� ����
	/// </summary>
	bool AttachToHolster(class USceneComponent* InParent) override;


	bool AttachToHand(class USceneComponent* InParent) override;

protected:

	const FName EQUIPPED_SOCKET_NAME = "Throwable_Equip";

};