// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_CombatFieldWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_CombatFieldWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:

	void SetOwnerCombatFieldManager(class AC_CombatFieldManager* InCombatFieldManager) { OwnerCombatFieldManager = InCombatFieldManager; }
	
private:

	AC_CombatFieldManager* OwnerCombatFieldManager{};

};
