// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "C_DragDropOperation.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_DragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UUserWidget* WidgetReference = nullptr;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)

};
