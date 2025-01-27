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

	void SetIsAttached(bool InIsAttachedToWeapon);
	void SetMeshVisibility(bool InIsVisible);

	bool AttachItemToWeaponAndMove(class AC_Gun* Weapon, class AC_BasicCharacter* Character);

	bool AttachItemToWeaponAndMoveInven(AC_Gun* Weapon, class UC_InvenComponent* invenComp);

	bool SwapAttachableItem(AC_Gun* CurrentGun, AC_Gun* OtherGun);

public:
	EAttachmentNames GetAttachmentName() { return AttachmentName; }

	AC_Gun* GetOwnerGun() { return OwnerGun; }

	void SetOwnerGun(class AC_Gun* InOwnerGun) { OwnerGun = InOwnerGun; }

	EPartsName GetName() { return Name; }
protected:
	bool MoveAroundToInven(AC_BasicCharacter* Character) override;
	bool MoveAroundToSlot(AC_BasicCharacter* Character) override;

	bool MoveInvenToAround(AC_BasicCharacter* Character) override;
	bool MoveInvenToSlot(AC_BasicCharacter* Character) override;

	bool MoveSlotToAround(AC_BasicCharacter* Character) override;
	bool MoveSlotToInven(AC_BasicCharacter* Character) override;
	bool MoveSlotToSlot(AC_BasicCharacter* Character) override;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EAttachmentNames AttachmentName{};

	AC_Gun* OwnerGun = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPartsName Name{};
private:
	class UCapsuleComponent* CapsuleCollider{};
	class USkeletalMeshComponent* AttachableItemMesh{};




};
