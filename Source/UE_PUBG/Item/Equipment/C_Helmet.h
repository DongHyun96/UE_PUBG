// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "C_Helmet.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_Helmet : public AC_EquipableItem
{
	GENERATED_BODY()

public:

	AC_Helmet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	bool Interaction(AC_BasicCharacter* Character) override;
	
	bool LegacyMoveToAround(AC_BasicCharacter* Character) override;

	bool LegacyMoveToSlot(AC_BasicCharacter* Character) override;

	void AttachToSocket(class AC_BasicCharacter* InParent);

private:
	
	bool MoveSlotToAround(AC_BasicCharacter* Character) override;

	bool MoveAroundToSlot(AC_BasicCharacter* Character) override;

public:

	bool TakeDamage(float DamageAmount) override;

	float GetDamageReduceFactor() const override;

private:

	class UStaticMeshComponent* HelmetMesh{};
	
};


