// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Blueprint/IUserObjectListEntry.h"
#include "Components/TextBlock.h"
#include "Components/Progressbar.h"

#include "Item/C_Item.h"

#include "UE_PUBG/Character/C_BasicCharacter.h"

#include "C_ItemBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_ItemBarWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
    // 이벤트 함수 선언
    // ListView에서 항목 객체가 설정될 때 호출되는 함수
    virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    virtual FReply NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

    virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);

    void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
    void InitBar(AC_Item* item);

	UFUNCTION(BlueprintCallable)
    class AC_Item* GetItem(AC_Item* nearItem);

    UFUNCTION(BlueprintCallable)
    AC_Item* DropItem(AC_Item* myItem);

    /// <summary>
    /// 아이템 사용시 눈으로 알려주는 progressbar타이머.
    /// </summary>
    /// <param name="curTime"></param>
    /// <param name="endTime"></param>
    UFUNCTION(BlueprintCallable)
    void SetPercent(float curTime, float endTime);

    UFUNCTION(BlueprintCallable)
    void InitInvenUIWidget();
    //두 기능을 합친 SetItem생각

    void SetOwnerCharacter(AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

public:
    // UI에 바인딩할 변수 선언 (Blueprint에서 연결)
    //UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (BindWidget))

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
    class UImage* ItemImage1;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
    class UTextBlock* ItemName1;

    //현재 UI에서 바인딩하지 않음.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
    class UTextBlock* ItemStackBlock1;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
    class UProgressBar* ItemUsingTimer{};

    //uint8 ItemStack;
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
    EItemTypes ItemType{};

    // C++에서 처리할 아이템
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
    AC_Item* CachedItem;
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    class AC_BasicCharacter* OwnerCharacter;
};
