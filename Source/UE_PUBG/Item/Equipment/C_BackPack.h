// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "C_BackPack.generated.h"

/**
 * �賶 (Level 1): ���� �뷮 +150
 * �賶 (Level 2): ���� �뷮 +200
 * �賶 (Level 3)(���� ���� 3���� �賶�� ����): ���� �뷮 +250
 */


UCLASS()
class UE_PUBG_API AC_BackPack : public AC_EquipableItem
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:

	AC_BackPack();

	void Interaction(class AC_BasicCharacter* character) override;

	uint8 GetLevel() { return Level; }
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 Level;
};
