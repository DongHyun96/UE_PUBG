// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvenUI/BasicItemSlot/C_BasicEquippedItemSlotWidget.h"
#include "C_AttachableItemSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_AttachableItemSlotWidget : public UC_BasicEquippedItemSlotWidget
{
	GENERATED_BODY()
	
public:
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
};
