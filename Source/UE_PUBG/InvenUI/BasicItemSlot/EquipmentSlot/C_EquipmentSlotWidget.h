// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvenUI/BasicItemSlot/C_BasicEquippedItemSlotWidget.h"
#include "Character/Component/C_InvenComponent.h"
#include "Item/Equipment/C_EquipableItem.h"
#include "Components/Image.h"
#include "C_EquipmentSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_EquipmentSlotWidget : public UC_BasicEquippedItemSlotWidget
{
	GENERATED_BODY()
public:

	virtual void NativeConstruct() override;

	/// <summary>
	/// 우클릭 이벤트
	/// </summary>
	/// <param name="InGeometry"></param>
	/// <param name="InMouseEvent"></param>
	/// <returns></returns>
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	virtual void UpdateWidget() override;


protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UImage* ItemImage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	EItemTypes SlotItemType{};
};
