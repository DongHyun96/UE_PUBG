// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvenUI/BasicItemSlot/EquipmentSlot/C_EquipmentSlotWidget.h"
#include "C_VestSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_VestSlotWidget : public UC_EquipmentSlotWidget
{
	GENERATED_BODY()

public:
	/// <summary>
	/// DurabilityBar(내구도bar)를 업데이트함.
	/// </summary>
	void UpdateDurabilityBar(float percent) override;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UProgressBar* DurabilityBar = nullptr;
};


