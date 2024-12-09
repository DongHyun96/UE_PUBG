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
    // �̺�Ʈ �Լ� ����
    // ListView���� �׸� ��ü�� ������ �� ȣ��Ǵ� �Լ�
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
    /// ������ ���� ������ �˷��ִ� progressbarŸ�̸�.
    /// </summary>
    /// <param name="curTime"></param>
    /// <param name="endTime"></param>
    UFUNCTION(BlueprintCallable)
    void SetPercent(float curTime, float endTime);

    UFUNCTION(BlueprintCallable)
    void InitInvenUIWidget();
    //�� ����� ��ģ SetItem����

    void SetOwnerCharacter(AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

public:
    // UI�� ���ε��� ���� ���� (Blueprint���� ����)
    //UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (BindWidget))

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
    class UImage* ItemImage1;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
    class UTextBlock* ItemName1;

    //���� UI���� ���ε����� ����.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
    class UTextBlock* ItemStackBlock1;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
    class UProgressBar* ItemUsingTimer{};

    //uint8 ItemStack;
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
    EItemTypes ItemType{};

    // C++���� ó���� ������
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
    AC_Item* CachedItem;
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    class AC_BasicCharacter* OwnerCharacter;
};
