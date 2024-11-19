// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UE_PUBG/Character/C_BasicCharacter.h"

#include "C_BasicUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BasicUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void SetOwnerCharacter(AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

protected:
	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class AC_BasicCharacter* OwnerCharacter = nullptr;
};
