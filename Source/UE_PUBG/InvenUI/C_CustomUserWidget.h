// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/C_Item.h"
#include "Character/C_Player.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "C_CustomUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_CustomUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	class AC_Player* GetOwnerPlayer() { return OwnerPlayer; }

	virtual void SetOwnerPlayer(AC_Player* InOwnerPlayer) { OwnerPlayer = InOwnerPlayer; }

protected:
	UPROPERTY()
	AC_Player* OwnerPlayer = nullptr;
};


