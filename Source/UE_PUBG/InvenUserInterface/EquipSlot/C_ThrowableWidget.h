// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InvenUserInterface/C_BasicUserWidget.h"
#include "UE_PUBG/Character/C_BasicCharacter.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "Character/Component/C_EquippedComponent.h"

#include "C_ThrowableWidget.generated.h"

class UImage;
class UTextBlock;
class AC_Weapon;
/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_ThrowableWidget : public UC_BasicUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void Init();

public:
	
	void SetWeapon(class AC_Item* item);

	void SetWeaponBoxNum(uint8 Num);

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UImage* ItemIcon = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UTextBlock* WeaponNum = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UTextBlock* ItemName = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AC_Weapon* Weapon = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EWeaponSlot WeaponSlotType{};

};
