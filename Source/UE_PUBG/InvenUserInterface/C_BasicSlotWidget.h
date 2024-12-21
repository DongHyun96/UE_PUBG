// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/C_Item.h"
#include "Character/C_Player.h"

#include "C_BasicSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BasicSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);

	void SetOwnerCharacter(class AC_Player* inOwnerCharacter) { OwnerCharacter = inOwnerCharacter; }

private:
	virtual void HandleDragWidgetStart();
protected:
	// C++에서 처리할 아이템
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AC_Item* CachedItem;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AC_Player* OwnerCharacter = nullptr;
};
