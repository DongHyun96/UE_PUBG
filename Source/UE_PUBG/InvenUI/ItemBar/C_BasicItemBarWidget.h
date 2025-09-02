// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Item/C_Item.h"
#include "C_BasicItemBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BasicItemBarWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:

	void NativeConstruct() override;

	/// <summary>
	/// ItemBar를 우클릭했을 때 이벤트 구현.(Item의 Interaction 함수 호출)
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
	/// 드래그 중인 ItemBar의 아이템을 감지.
	/// </summary>
	/// <param name="InGeometry"></param>
	/// <param name="InMouseEvent"></param>
	/// <param name="OutOperation"></param>
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	// 이벤트 함수 선언
	// ListView에서 항목 객체가 설정될 때 호출되는 함수
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;


	void UpdateWidget(AC_Item* MyItem);
	void UpdateWidget(class UC_ItemDataObject* DataObj);


	//void UpdateWidget();


	/// <summary>
    /// 아이템 사용시 눈으로 알려주는 progressbar타이머.
    /// </summary>
    /// <param name="curTime"></param>
    /// <param name="endTime"></param>
    UFUNCTION(BlueprintCallable)
    void SetPercent(float curTime, float endTime);

	/// <summary>
	/// Interaction후 InvenUI를 업데이트하는 함수.
	/// </summary>
	void UpdateInvenUIWidget();

	/// <summary>
	/// alt + 우클릭으로 아이템의 절반을 줍거나 버릴 때 사용.
	/// </summary>
	bool HalfStackItemInteraction();

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UImage* ItemImage = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UTextBlock* ItemName = nullptr;

	//현재 UI에서 바인딩하지 않음.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UTextBlock* ItemStackBlock = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UProgressBar* ItemUsingTimer = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	EItemTypes ItemType{};

	// C++에서 처리할 아이템
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AC_Item* CachedItem = nullptr;

	UC_ItemDataObject* DataObj = nullptr;
	//에디터에서 바인딩으로 처리한다면?
	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//const FItemData* CachedItemData = nullptr; 

};


