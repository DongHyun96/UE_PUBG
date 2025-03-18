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
		//Offset = FVector2D(-32, 32);
		curWeaponSlot = EWeaponSlot::NONE;
	}

	void SetDraggedItemBox(class AC_Item* InItem);

	void SetDraggedItemBox(class UC_ItemBox* InItemBox);


public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UUserWidget* WidgetReference = nullptr;

	UPROPERTY(BlueprintReadWrite)
	class AC_Item* DraggedItem = nullptr;

	//UPROPERTY(BlueprintReadWrite)
	class UC_ItemBox* DraggedItemBox = nullptr;


	UPROPERTY(BlueprintReadWrite)
	EWeaponSlot curWeaponSlot = EWeaponSlot::NONE;
};


