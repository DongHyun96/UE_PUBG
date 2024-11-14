// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"


#include "C_InvenUiWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_InvenUiWidget : public UUserWidget
{
	GENERATED_BODY()
public:
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

	UFUNCTION(BlueprintCallable)
	void SetItemListZorder(AC_BasicCharacter* Character);

	UFUNCTION(BlueprintCallable)
	void InitItemListZorder();
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
	UUserWidget* Background_Around = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UUserWidget* Background_Inventory = nullptr;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AC_BasicCharacter* OwnerCharacter = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsDragging = false;

};
