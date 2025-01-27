// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvenUI/BasicItemSlot/WeaponSlot/C_WeaponSlotWidget.h"
#include "C_GunSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_GunSlotWidget : public UC_WeaponSlotWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	bool MouseRBDownInteraction(AC_Weapon* inSlotWeapon) override;

	void UpdateWidget() override;

	void UpdateAttachableSlotVisibility();
	
	bool SetAttachmentSlotOnDrop(AC_Weapon* InSlotWeapon, class AC_AttachableItem* InAttachableItem);
protected:
	virtual bool HandleDrop(class UC_DragDropOperation* InOperation); //TODO: 순수가상함수로

	virtual bool ChangedGunSlot(class AC_Gun* gun);
	
private:
	void InitializeAttachSlotMap();

protected:

	TMap<EPartsName, class UC_AttachableItemSlotWidget*> AttachSlotWidgets;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UC_AttachableItemSlotWidget* WB_MuzzleSlot = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UC_AttachableItemSlotWidget* WB_MagazineSlot = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UC_AttachableItemSlotWidget* WB_ScopeSlot = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UC_AttachableItemSlotWidget* WB_StockSlot = nullptr;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UC_AttachableItemSlotWidget* WB_GripSlot = nullptr;
};
