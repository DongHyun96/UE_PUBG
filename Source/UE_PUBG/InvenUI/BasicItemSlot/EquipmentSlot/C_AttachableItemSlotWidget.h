// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvenUI/BasicItemSlot/C_BasicEquippedItemSlotWidget.h"
#include "C_AttachableItemSlotWidget.generated.h"

//UENUM(BlueprintType)
enum class EWeaponSlot : uint8;

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_AttachableItemSlotWidget : public UC_BasicEquippedItemSlotWidget
{
	GENERATED_BODY()
	
public:
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

	void UpdateSlotItemImage(class AC_Gun* SlotItem);

	/// <summary>
	/// C_GunSlotWidget���� ���Կ� ������ �ѱ��� �������� ����� �Ű������� �Ѱ��ָ�
	/// �ش� �Լ��� ���� ��� �ȿ� �ڽ��� SlotName�� �ִ��� Ȯ���ϰ� �ִٸ� Ȱ��ȭ �ȴ�.
	/// ���ٸ� ��Ȱ��ȭ(Collapsed)�ȴ�.
	/// </summary>
	/// <param name="AttachableSlots"></param>
	void DiscriminateSlot(TArray<EPartsName> AttachableSlots, AC_Gun* SlotItem);

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	EPartsName SlotName{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UImage* ItemImage = nullptr;

	//Main,SubGunSlot�� ������? 
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EWeaponSlot GunSlot{};
};
