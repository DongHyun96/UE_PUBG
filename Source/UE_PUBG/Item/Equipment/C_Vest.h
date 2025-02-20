// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "C_Vest.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_Vest : public AC_EquipableItem
{
	GENERATED_BODY()
	
public:
	AC_Vest();

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

public:
	/// <summary>
	/// EquipableItemLevel에 따른 ItemDatas 및 StaticMesh 초기화 하기
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void InitVestDatasAndStaticMesh();

};


