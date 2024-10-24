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
	void InitWidget();
protected:
	//void PreConstruct(bool IsDesignTime) override;

	virtual void NativeConstruct() override;

	/// <summary>
	/// 위젯 초기화
	/// </summary>


	void PopulateItemList(class UListView* list, const TMap<FString, class AC_Item*>& itemList);

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UCanvasPanel* InvenCanvas = nullptr;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UC_MyItemListWidget* MyItemListWidget = nullptr; // 아이템 리스트 위젯

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UC_MyItemListWidget* AroundItemListWidget = nullptr; // 아이템 리스트 위젯

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
