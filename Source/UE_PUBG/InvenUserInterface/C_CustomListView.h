// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ListView.h"
#include "UE_PUBG/Character/C_BasicCharacter.h"

#include "C_CustomListView.generated.h"


/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_CustomListView : public UListView
{
	GENERATED_BODY()
protected:

	void Init();

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class AC_BasicCharacter* OwnerCharacter;
};