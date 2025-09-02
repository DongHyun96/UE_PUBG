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

	virtual void NativeConstruct() override;

	void SetVisibility(ESlateVisibility InVisibility) override;

	void SetOwnerPlayer(AC_Player* InOwnerPlayer) override;

	/// <summary>
	/// 힐템 사용시 Left to Right Progress Bar로 사용 진행도를 시각화.
	/// </summary>
	/// <param name="Character"></param>
	UFUNCTION(BlueprintCallable)
	void UpdateVolumeBar(AC_BasicCharacter* Character);

	/// <summary>
	/// InvenUI를 전체적으로 Update.
	/// </summary>
	void UpdateWidget();

	/// <summary>
	/// AroundPanel을 Update
	/// </summary>
	void UpdateAroundItemPanelWidget();

	/// <summary>
	/// InventoryPanel을 Update
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void UpdateInventroyItemPanelWidget();

	/// <summary>
	/// EquipmentPanel을 update
	/// </summary>
	void UpdateEquipmentItemPanelWidget();

	/// <summary>
	/// UC_BasicPanelWidget에서 DivideItemPanel이 열릴 때 뒤의 Around, Inventory Panel등의 상호작용을 막기 위한 함수.
	/// 이 함수에서 DivideItemPanel을 여는 기능은 없음.
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void MainPanelsSetVisivility(ESlateVisibility InVisibility);

	/// <summary>
	/// 이 함수로 DivideItemPanelWidget들을 한번에 닫음.
	/// </summary>
	void CloseDivideItemWidget();

	/// <summary>
	/// 플레이어가 근처의 아이템과 멀어져서 더 이상 줍는게 안되는 상태가 되면 AroundItemList에서 더 이상 안보이도록 제거하는 함수.
	/// </summary>
	/// <param name="InItem"></param>
	void RemoveItemInList(AC_Item* InItem);

	void UpdateAroundItemList(AC_Item* InItem);

public:
	bool GetIsDragging() { return bIsDragging; }

	bool GetIsPanelOpened();

	AC_Item* GetUsingItem() { return UsingItem; }

	bool SetIsDragging(bool Dragging) { return bIsDragging = Dragging; }

	void SetUsingItem(AC_Item* inItem) { UsingItem = inItem; }

	class UC_BasicDivideItemPanelWidget* GetDivideItemAroundDropWidget() { return DivideItemAroundDropWidget; }

	class UC_BasicDivideItemPanelWidget* GetDivideItemInventoryDropWidget() { return DivideItemInventoryDropWidget; }

	class UC_EquipmentPanelWidget* GetEquipmentPanel() { return EquipmentPanel; }

protected:



protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UC_AroundItemPanelWidget* AroundItemPanel = nullptr; // 외부 아이템 리스트 위젯

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UC_InventoryItemPanelWidget* InventoryPanel = nullptr; // 나의 아이템 리스트 위젯

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UC_EquipmentPanelWidget* EquipmentPanel = nullptr;

	/// <summary>
	/// 현재 사용중인 용량(아이템의 Volume의 합)
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UProgressBar* CurVolumeBar = nullptr;

	/// <summary>
	/// 현재 최대 용량
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UProgressBar* MaxVolumeBar = nullptr;

	bool bIsDragging = false;

	/// <summary>
	/// 사용중인 힐템.
	/// </summary>
	AC_Item* UsingItem = nullptr;


	//나눠 버리는 위젯
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UC_BasicDivideItemPanelWidget* DivideItemAroundDropWidget = nullptr;

	//나눠 줍는 위젯
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UC_BasicDivideItemPanelWidget* DivideItemInventoryDropWidget = nullptr;
};


