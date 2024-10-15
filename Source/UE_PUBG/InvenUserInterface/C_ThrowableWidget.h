// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UE_PUBG/Character/C_BasicCharacter.h"

#include "C_ThrowableWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_ThrowableWidget : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class AC_BasicCharacter* OwnerCharacter;
};
