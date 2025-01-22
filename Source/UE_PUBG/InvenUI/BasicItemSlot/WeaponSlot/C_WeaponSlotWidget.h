// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvenUI/BasicItemSlot/C_BasicEquippedItemSlotWidget.h"
#include "Item/Weapon/C_Weapon.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

#include "Character/Component/C_EquippedComponent.h"

#include "C_WeaponSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_WeaponSlotWidget : public UC_BasicEquippedItemSlotWidget
{
	GENERATED_BODY()

public:
	/// <summary>
	/// ��Ŭ�� �̺�Ʈ.
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
	/// �巡���� ����Ǵ� �Լ�.
	/// </summary>
	/// <param name="InGeometry"></param>
	/// <param name="InMouseEvent"></param>
	/// <param name="OutOperation"></param>
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	/// <summary>
	/// �� ������ Update ȣ�� �Լ�.
	/// </summary>
	virtual void UpdateWidget() override;

	/// <summary>
	/// �������� �ִٸ� �̹����� �����ְ�(visible) �ƴ϶�� �����.(Hidden)
	/// </summary>
	/// <param name="SlotItem"></param>
	void UpdateSlotItemImage(AC_Weapon* SlotItem); 

	/// <summary>
	/// Slot�� ��Ŭ�� �̺�Ʈ���� ���� �Լ�, �ڽ� Ŭ�������� �����ȴ�.
	/// </summary>
	/// <param name="inSlotWeapon"></param>
	/// <returns></returns>
	virtual bool MouseRBDownInteraction(AC_Weapon* inSlotWeapon); //TODO : ��������� ��ô���� �κ��� ������ ������ ���� ó�� Ȯ���ϱ�.

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UImage* WeaponImage = nullptr;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	//UImage* Background_Boundery = nullptr;

	//WeaponBoxNum�� �������� �ذ��ϱ�.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UTextBlock* WeaponNum = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UTextBlock* WeaponName = nullptr;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UCanvasPanel* ButtonPanel = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UCanvasPanel* WeaponSlotPanel = nullptr;
	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//AC_Weapon* Weapon = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EWeaponSlot WeaponType{};
};
