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
	/// �����۸���Ʈ(ListView)�ʱ�ȭ
	/// </summary>
	void InitListView();
	void InitWidget();
protected:
	//void PreConstruct(bool IsDesignTime) override;

	virtual void NativeConstruct() override;

	/// <summary>
	/// ���� �ʱ�ȭ
	/// </summary>


	void PopulateItemList(class UListView* list, const TMap<FString, class AC_Item*>& itemList);

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UCanvasPanel* InvenCanvas = nullptr;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UC_MyItemListWidget* MyItemListWidget = nullptr; // ������ ����Ʈ ����

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UC_MyItemListWidget* AroundItemListWidget = nullptr; // ������ ����Ʈ ����

	//ListViewWidget
	class UListView* MyItemListView = nullptr;

	UListView* AroundItemListView = nullptr;

	//EquipWidget
	class UC_MainGunWidget* MainGunSlot = nullptr;

	UC_MainGunWidget* SubGunSlot = nullptr;

	class UC_ThrowableWidget* MeleeSlot = nullptr;

	UC_ThrowableWidget* ThrowableSlot = nullptr;


protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AC_BasicCharacter* OwnerCharacter = nullptr;
};
