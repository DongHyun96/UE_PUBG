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
	
	//bool LegacyMoveToAround(AC_BasicCharacter* Character) override;
	//
	//bool LegacyMoveToSlot(AC_BasicCharacter* Character) override;

	void AttachToSocket(class AC_BasicCharacter* InParent);

protected:
	
	bool MoveSlotToAround(AC_BasicCharacter* Character, int32 InStack) override;

	bool MoveAroundToSlot(AC_BasicCharacter* Character, int32 InStack) override;

public:

	bool TakeDamage(float DamageAmount) override;

	float GetDamageReduceFactor() const override;

	/// <summary>
	/// EquipableItemLevel에 따른 Static DamageReduceFactor 구하기
	/// </summary>
	static float GetDamageReduceFactor(EEquipableItemLevel EquipableItemLevel);

private:

	class UStaticMeshComponent* HelmetMesh{};
	
};


