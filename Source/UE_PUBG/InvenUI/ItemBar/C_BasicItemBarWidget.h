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
	/// ItemBar�� ��Ŭ������ �� �̺�Ʈ ����.(Item�� Interaction �Լ� ȣ��)
	/// </summary>
	/// <param name="InGeometry"></param>
	/// <param name="InMouseEvent"></param>
	/// <returns></returns>
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/// <summary>
	/// ��Ŭ������ �巡�� �̺�Ʈ �̺�Ʈ�� ����.
	/// </summary>
	/// <param name="InGeometry"></param>
	/// <param name="InMouseEvent"></param>
	/// <returns></returns>
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);

	/// <summary>
	/// �巡�� ���� ItemBar�� �������� ����.
	/// </summary>
	/// <param name="InGeometry"></param>
	/// <param name="InMouseEvent"></param>
	/// <param name="OutOperation"></param>
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	// �̺�Ʈ �Լ� ����
	// ListView���� �׸� ��ü�� ������ �� ȣ��Ǵ� �Լ�
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	void UpdateWidget(AC_Item* MyItem);

	/// <summary>
    /// ������ ���� ������ �˷��ִ� progressbarŸ�̸�.
    /// </summary>
    /// <param name="curTime"></param>
    /// <param name="endTime"></param>
    UFUNCTION(BlueprintCallable)
    void SetPercent(float curTime, float endTime);

	/// <summary>
	/// Interaction�� InvenUI�� ������Ʈ�ϴ� �Լ�.
	/// </summary>
	void UpdateInvenUIWidget();
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UImage* ItemImage = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UTextBlock* ItemName = nullptr;

	//���� UI���� ���ε����� ����.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UTextBlock* ItemStackBlock = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UProgressBar* ItemUsingTimer = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	EItemTypes ItemType{};

	// C++���� ó���� ������
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AC_Item* CachedItem;

};
