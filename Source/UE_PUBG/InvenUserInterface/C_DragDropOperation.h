// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Character/Component/C_EquippedComponent.h"
#include "C_DragDropOperation.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_DragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	UC_DragDropOperation()
	{
		curWeaponSlot = EWeaponSlot::NONE;
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UUserWidget* WidgetReference = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UObject* DraggedItem;


	UPROPERTY(BlueprintReadWrite)
	EWeaponSlot curWeaponSlot = EWeaponSlot::NONE;
};
