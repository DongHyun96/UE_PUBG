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
	/// ��Ŭ�� �̺�Ʈ
	/// </summary>
	/// <param name="InGeometry"></param>
	/// <param name="InMouseEvent"></param>
	/// <returns></returns>
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/// <summary>
	/// ��Ŭ������ �巡�� �̺�Ʈ �̺�Ʈ�� ����.
	/// </summary>
	/// <param name="InGeometry"></param>
	/// <param name="InMouseEvent"></param>
	/// <returns></returns>
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);

	/// <summary>
	/// �巡�� ���� Slot�� �������� ����.
	/// </summary>
	/// <param name="InGeometry"></param>
	/// <param name="InMouseEvent"></param>
	/// <param name="OutOperation"></param>
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	virtual void UpdateWidget() override;

protected:
	void UpdateSlotItemImage(AC_EquipableItem* SlotItem);


protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UImage* ItemImage = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	EEquipSlot EquipSlot{};
};
