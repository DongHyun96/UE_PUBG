// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InvenuserInterface/C_MainGunWidget.h"
#include "InvenuserInterface/C_ThrowableWidget.h"
#include "InvenuserInterface/C_EquipSlot.h"

#include "C_EquipmentPanel.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_EquipmentPanel : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void InitializeWidget();

	void SetOwnerChracter(AC_BasicCharacter* inOwnerCharacter) { OwnerCharacter = inOwnerCharacter; }

	void SetWidgetOwnerCharacter(AC_BasicCharacter* inOwnerCharacter);
private:

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UC_MainGunWidget* MainGunWidget = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UC_MainGunWidget* SubGunWidget = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UC_ThrowableWidget* MeleeWidget = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UC_ThrowableWidget* ThrowableWidget = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	class UC_EquipSlot* HelmetWidget = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UC_EquipSlot* BackPackWidget = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UC_EquipSlot* ArmorWidget = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UC_EquipSlot* UnKnownSlot = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AC_BasicCharacter* OwnerCharacter = nullptr;
};
