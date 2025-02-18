// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "Blueprint/UserWidget.h"
#include "C_BasicSlotWidget.h"
#include "UE_PUBG/Character/C_BasicCharacter.h"

#include "C_EquipSlot.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_EquipSlot : public UC_BasicSlotWidget
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

	void Init();

	void DivisionInit();

	//void SetOwnerCharacter(AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

	void SetSlotItemType(EItemTypes ItemType) { SlotItemType = ItemType; }
protected:
	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//class AC_BasicCharacter* OwnerCharacter = nullptr;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//class AC_EquipableItem* EquippedItem = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UImage* ItemImage1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	EItemTypes SlotItemType{};
};


