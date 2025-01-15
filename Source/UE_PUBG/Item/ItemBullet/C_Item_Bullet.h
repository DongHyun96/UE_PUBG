// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/C_Item.h"
#include "C_Item_Bullet.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EBulletType : uint8
{
	FIVEMM,
	SEVENMM,
	NONE
};
UCLASS()
class UE_PUBG_API AC_Item_Bullet : public AC_Item
{
	GENERATED_BODY()
public:
	AC_Item_Bullet();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	bool Interaction(AC_BasicCharacter* Character) override;
	bool MoveAroundToInven(AC_BasicCharacter* Character) override;
	bool MoveInvenToAround(AC_BasicCharacter* Character) override;
	void AddBulletStackToCharacter();
protected:
	UPROPERTY(BluePrintReadWrite, EditAnywhere)

	EBulletType CurBulletType;
};
