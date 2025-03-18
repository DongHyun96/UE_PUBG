// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "C_ItemBox.generated.h"


class AC_Item;
/**
 * 
 */
UCLASS(BlueprintType)
class UE_PUBG_API UC_ItemBox : public UObject
{
	GENERATED_BODY()
public:

    /** 초기화 함수 */
    void Init(AC_Item* InItem, int32 InStack)
    {
        ItemRef = InItem;
        StackCount = InStack;
    }
	UFUNCTION(BlueprintCallable)
    AC_Item* GetItemRef() { return ItemRef; }

    UFUNCTION(BlueprintCallable)
    int32 GetItemStackCount() { return StackCount; }

protected:
    /** UObject는 new로 직접 생성할 수 없으므로 생성자 보호 */
    UC_ItemBox() {}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
    AC_Item* ItemRef = nullptr;

    /** UI에서 보여줄 개수 (MaxStack 적용) */
    UPROPERTY()
    int32 StackCount = 0;
};
