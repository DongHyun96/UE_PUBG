// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Utility/CommonTypes.h"
#include "C_ItemDataObject.generated.h"



class AC_BasicCharacter;
struct FItemData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemDataChanged, class UC_ItemDataObject*, ItemData);
/**
 UI와 아이템 데이터 간의 중개자 역할을 하는 UObject 클래스.
 **/
UCLASS(BlueprintType)
class UE_PUBG_API UC_ItemDataObject : public UObject
{
    GENERATED_BODY()

public:
    // 이벤트: 값이 바뀔 때 브로드캐스트
    UPROPERTY(BlueprintAssignable, Category = "Item|Event")
    FOnItemDataChanged OnItemDataChanged;

    // FItemData 참조 (읽기 전용)
    const FItemData* ItemDataRef = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Item")
    AC_Item* ItemRef = nullptr;

private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (AllowPrivateAccess = "true"))
    FName ItemCode = "NONE";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (AllowPrivateAccess = "true"))
    EItemPlace ItemPlace = EItemPlace::AROUND;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (AllowPrivateAccess = "true"))
    int32 ItemCurStack = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (AllowPrivateAccess = "true"))
    AC_BasicCharacter* OwnerCharacter{};

public:
    // Getter / Setter
    FName GetItemCode() const { return ItemCode; }
    void SetItemCode(FName NewCode)
    {
        if (ItemCode != NewCode)
        {
            ItemCode = NewCode;
            OnItemDataChanged.Broadcast(this);
        }
    }

    EItemPlace GetItemPlace() const { return ItemPlace; }
    void SetItemPlace(EItemPlace NewPlace)
    {
        if (ItemPlace != NewPlace)
        {
            ItemPlace = NewPlace;
            OnItemDataChanged.Broadcast(this);
        }
    }

    int32 GetItemCurStack() const { return ItemCurStack; }
    void SetItemCurStack(int32 NewStack)
    {
        if (ItemCurStack != NewStack)
        {
            ItemCurStack = NewStack;
            OnItemDataChanged.Broadcast(this);
        }
    }

    AC_BasicCharacter* GetOwnerCharacter() const { return OwnerCharacter; }
    void SetOwnerCharacter(AC_BasicCharacter* NewOwner)
    {
        if (OwnerCharacter != NewOwner)
        {
            OwnerCharacter = NewOwner;
            OnItemDataChanged.Broadcast(this);
        }
    }
};
