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

	UFUNCTION(BlueprintCallable)
	void SetOwnerCharacter(class AC_BasicCharacter* Character) { OwnerCharacter = Character; }

	UFUNCTION(BlueprintCallable)
	void SetWidgetsOwner(AC_BasicCharacter* Character);

	/// <summary>
	/// �����۸���Ʈ(ListView)�ʱ�ȭ
	/// </summary>
	void InitListView();

	UFUNCTION(BlueprintCallable)
	void InitWidget(); 

	bool SetIsDragging(bool Dragging) { return bIsDragging = Dragging; }


public:
	bool GetIsDragging() { return bIsDragging; }
protected:
	
	//virtual void PreConstruct(bool IsDesignTime) override;

	virtual void NativeConstruct() override;

	/// <summary>
	/// ���� �ʱ�ȭ
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
	class UC_MyItemListWidget* MyItemListWidget = nullptr; // ������ ����Ʈ ����

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UC_MyItemListWidget* AroundItemListWidget = nullptr; // ������ ����Ʈ ����

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

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AC_BasicCharacter* OwnerCharacter = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsDragging = false;

};
