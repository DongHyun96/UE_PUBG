// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/ConsumableItem/C_ConsumableItem.h"
#include "C_InvenUiWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_InvenUiWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// Called every frame
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetOwnerCharacter(class AC_BasicCharacter* Character) { OwnerCharacter = Character; }

	UFUNCTION(BlueprintCallable)
	void SetWidgetsOwner(AC_BasicCharacter* Character);

	/// <summary>
	/// 아이템리스트(ListView)초기화
	/// </summary>
	void InitListView();

	UFUNCTION(BlueprintCallable)
	void InitWidget(); 

	bool SetIsDragging(bool Dragging) { return bIsDragging = Dragging; }

	void BindToItemState(class AC_Item* Item);

	void SetUsingItem(AC_Item* inItem) { UsingItem = inItem; }

private:
	UFUNCTION()
	void HandleItemStateChanged(EConsumableItemState NewState);
public:
	bool GetIsDragging() { return bIsDragging; }
protected:
	
	//virtual void PreConstruct(bool IsDesignTime) override;

	virtual void NativeConstruct() override;

	/// <summary>
	/// 위젯 초기화
	/// </summary>

	UFUNCTION(BlueprintCallable)
	void PopulateItemList(class UListView* list, const TMap<FString, class AC_Item*>& itemList);

	UFUNCTION(BlueprintCallable)
	void testAroundItemList(class UListView* list, const TArray<AC_Item*>& AroundAtemList);

	UFUNCTION(BlueprintCallable)
	void UpdateVolumeBar(AC_BasicCharacter* Character);

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UCanvasPanel* InvenCanvas = nullptr;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UC_MyItemListWidget* MyItemListWidget = nullptr; // 아이템 리스트 위젯

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UC_MyItemListWidget* AroundItemListWidget = nullptr; // 아이템 리스트 위젯

	//ListViewWidget
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UListView* MyItemListView = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UListView* AroundItemListView = nullptr;

	//EquipWidget
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UC_MainGunWidget* MainGunSlot = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UC_MainGunWidget* SubGunSlot = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UC_ThrowableWidget* MeleeSlot = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UC_ThrowableWidget* ThrowableSlot = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UC_EquipSlot* BackPackSlot = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UProgressBar* curVolumeBar = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UProgressBar* maxVolumeBar = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UC_EquipmentPanel* EquipmentPanel = nullptr;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AC_BasicCharacter* OwnerCharacter = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsDragging = false;

	AC_Item* UsingItem = nullptr;
};
