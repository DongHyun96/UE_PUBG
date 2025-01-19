// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvenUI/C_CustomUserWidget.h"
#include "C_InventoryUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_InventoryUIWidget : public UC_CustomUserWidget
{
	GENERATED_BODY()
public:


	void SetVisibility(ESlateVisibility InVisibility) override;

	void SetOwnerPlayer(AC_Player* InOwnerPlayer) override;

	//void SetWidgetsOwner(AC_BasicCharacter* Character);

	void UpdateVolumeBar(AC_BasicCharacter* Character);

	void InitializeListView();

	void UpdateWidget();

public:
	bool GetIsDragging() { return bIsDragging; }

	bool GetIsPanelOpened();

	AC_Item* GetUsingItem() { return UsingItem; }

	bool SetIsDragging(bool Dragging) { return bIsDragging = Dragging; }

	void SetUsingItem(AC_Item* inItem) { UsingItem = inItem; }

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UC_AroundItemPanelWidget* AroundItemPanel = nullptr; // 아이템 리스트 위젯

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UC_InventoryItemPanelWidget* InventoryPanel = nullptr; // 아이템 리스트 위젯

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UC_EquipmentPanelWdiget* EquipmentPanel = nullptr;

	//EquipWidget
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	//class UC_GunSlotWidget* MainGunSlot = nullptr;
	//
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	//UC_GunSlotWidget* SubGunSlot = nullptr;
	//
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	//class UC_MeleeWeaponSlotWidget* MeleeSlot = nullptr;
	//
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	//class UC_ThrowableWeaponSlotWidget* ThrowableSlot = nullptr;
	//
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	//class UC_EquipmentSlotWidget* BackPackSlot = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UProgressBar* CurVolumeBar = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UProgressBar* MaxVolumeBar = nullptr;

	bool bIsDragging = false;

	AC_Item* UsingItem = nullptr;
};
