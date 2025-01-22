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
	/// 우클릭 이벤트.
	/// </summary>
	/// <param name="InGeometry"></param>
	/// <param name="InMouseEvent"></param>
	/// <returns></returns>
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/// <summary>
	/// 좌클릭으로 드래그 이벤트 이벤트를 시작.
	/// </summary>
	/// <param name="InGeometry"></param>
	/// <param name="InMouseEvent"></param>
	/// <returns></returns>
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);

	/// <summary>
	/// 드래그중 실행되는 함수.
	/// </summary>
	/// <param name="InGeometry"></param>
	/// <param name="InMouseEvent"></param>
	/// <param name="OutOperation"></param>
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	/// <summary>
	/// 이 위젯의 Update 호출 함수.
	/// </summary>
	virtual void UpdateWidget() override;

	/// <summary>
	/// 아이템이 있다면 이미지를 보여주고(visible) 아니라면 숨긴다.(Hidden)
	/// </summary>
	/// <param name="SlotItem"></param>
	void UpdateSlotItemImage(AC_Weapon* SlotItem); 

	/// <summary>
	/// Slot의 우클릭 이벤트에서 사용될 함수, 자식 클래스에서 구현된다.
	/// </summary>
	/// <param name="inSlotWeapon"></param>
	/// <returns></returns>
	virtual bool MouseRBDownInteraction(AC_Weapon* inSlotWeapon); //TODO : 근접무기와 투척류가 인벤에 공간이 부족할 때의 처리 확인하기.

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UImage* WeaponImage = nullptr;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	//UImage* Background_Boundery = nullptr;

	//WeaponBoxNum은 블프에서 해결하기.
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
