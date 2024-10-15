// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Blueprint/IUserObjectListEntry.h"
#include "Components/TextBlock.h"

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

    void NativeConstruct();

	UFUNCTION(BlueprintCallable)
    void InitBar(AC_Item* item);

	UFUNCTION(BlueprintCallable)
    class AC_Item* GetItem(AC_Item* nearItem);

    UFUNCTION(BlueprintCallable)
    AC_Item* DropItem(AC_Item* myItem);

    //두 기능을 합친 SetItem생각

public:
    // UI에 바인딩할 변수 선언 (Blueprint에서 연결)
    UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (BindWidget))
    class UImage* ItemImage;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (BindWidget))
    class UTextBlock* ItemName;

    //현재 UI에서 바인딩하지 않음.
    UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (BindWidget))
    class UTextBlock* ItemStackBlock;

    //uint8 ItemStack;
    UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (BindWidget))
    EItemTypes ItemType{};

    // C++에서 처리할 아이템
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
    AC_Item* CachedItem;
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    class AC_BasicCharacter* OwnerCharacter;
};
