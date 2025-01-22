// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/C_Item.h"
#include "C_AttachableItem.generated.h"



/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_AttachableItem : public AC_Item
{
	GENERATED_BODY()
public:
	AC_AttachableItem();


	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	bool Interaction(AC_BasicCharacter* Character) override;
	bool MoveAroundToInven(AC_BasicCharacter* Character) override;
	bool MoveInvenToAround(AC_BasicCharacter* Character) override;

	bool MoveInvenToSlot(AC_BasicCharacter* Character);
	bool MoveSlotToInven(AC_BasicCharacter* Character);
	bool MoveSlotToSlot(AC_BasicCharacter* Character);


	void SetIsAttached(bool InIsAttachedToWeapon);
	void SetMeshVisibility(bool InIsVisible);
private:
	EPartsName Name;
	class UCapsuleComponent* CapsuleCollider{};
	class USkeletalMeshComponent* AttachableItemMesh{};
};
