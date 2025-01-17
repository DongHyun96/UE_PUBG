// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvenUI/BasicItemSlot/C_BasicEquippedItemSlotWidget.h"

#include "Item/Weapon/C_Weapon.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

#include "C_WeaponSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_WeaponSlotWidget : public UC_BasicEquippedItemSlotWidget
{
	GENERATED_BODY()

public:
	virtual void UpdateWidget() override;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UImage* WeaponImage = nullptr;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	//UImage* Background_Boundery = nullptr;

	//WeaponBoxNum은 블프에서 해결하기.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UTextBlock* WeaponNum = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UTextBlock* WeaponName = nullptr;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//AC_Weapon* Weapon = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EWeaponSlot WeaponType{};
};
