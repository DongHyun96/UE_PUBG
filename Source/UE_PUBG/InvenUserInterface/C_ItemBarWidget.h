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
    // �̺�Ʈ �Լ� ����
    // ListView���� �׸� ��ü�� ������ �� ȣ��Ǵ� �Լ�
    virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

    void NativeConstruct();

	UFUNCTION(BlueprintCallable)
    void InitBar(AC_Item* item);

	UFUNCTION(BlueprintCallable)
    class AC_Item* GetItem(AC_Item* nearItem);

    UFUNCTION(BlueprintCallable)
    AC_Item* DropItem(AC_Item* myItem);

    //�� ����� ��ģ SetItem����

public:
    // UI�� ���ε��� ���� ���� (Blueprint���� ����)
    UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (BindWidget))
    class UImage* ItemImage;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (BindWidget))
    class UTextBlock* ItemName;

    //���� UI���� ���ε����� ����.
    UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (BindWidget))
    class UTextBlock* ItemStackBlock;

    //uint8 ItemStack;
    UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (BindWidget))
    EItemTypes ItemType{};

    // C++���� ó���� ������
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
    AC_Item* CachedItem;
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    class AC_BasicCharacter* OwnerCharacter;
};
