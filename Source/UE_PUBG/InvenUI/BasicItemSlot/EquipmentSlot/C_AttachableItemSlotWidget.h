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
	/// C_GunSlotWidget에서 슬롯에 장착된 총기의 부착가능 목록을 매개변수로 넘겨주면
	/// 해당 함수를 통해 목록 안에 자신의 SlotName이 있는지 확인하고 있다면 활성화 된다.
	/// 없다면 비활성화(Collapsed)된다.
	/// </summary>
	/// <param name="AttachableSlots"></param>
	void DiscriminateSlot(TArray<EPartsName> AttachableSlots, AC_Gun* SlotItem);

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	EPartsName SlotName{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UImage* ItemImage = nullptr;
};
