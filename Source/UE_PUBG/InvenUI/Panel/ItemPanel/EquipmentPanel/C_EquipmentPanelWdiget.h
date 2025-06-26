// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvenUI/Panel/C_BasicPanelWidget.h"
#include "C_EquipmentPanelWdiget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_EquipmentPanelWdiget : public UC_BasicPanelWidget
{
	GENERATED_BODY()
public:
	bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	void UpdateWidget();

public:
	void SetOwnerPlayer(class AC_Player* InOwnerPlayer) override;

	class UC_MainGunSlotWidget* GetMainGunSlot() { return MainGunSlot; }

	class UC_SubGunSlotWidget* GetSubGunSlot() { return SubGunSlot; }
protected:
	bool HandleDrop(class AC_Item* DroppedItemBox) override;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UC_MainGunSlotWidget*	MainGunSlot = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UC_SubGunSlotWidget* SubGunSlot = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UC_MeleeWeaponSlotWidget* MeleeSlot = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UC_ThrowableWeaponSlotWidget* ThrowableSlot = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UC_HelmetSlotWidget* HelmetSlot = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UC_BackPackSlotWidget* BackPackSlot = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UC_VestSlotWidget* VestSlot = nullptr;
};


