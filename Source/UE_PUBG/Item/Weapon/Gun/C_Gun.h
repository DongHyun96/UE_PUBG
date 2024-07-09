// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/Weapon/C_Weapon.h"
#include "C_Gun.generated.h"

UCLASS()
class UE_PUBG_API AC_Gun : public AC_Weapon
{
	GENERATED_BODY()
	
public:	
	AC_Gun();

	bool AttachToHolster(class USceneComponent* InParent) override;
	bool AttachToHand(class USceneComponent* InParent) override;

};
