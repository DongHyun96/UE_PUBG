// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "UE_PUBG/Character/C_BasicCharacter.h"
#include "C_ItemListWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_ItemListWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    void NativeConstruct() override;

public:
    UFUNCTION(BlueprintCallable)
    void AddTMapItem(TMap<FString, AC_Item*> itemlist);
public:

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
    UListView* ItemListBar;



protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
    class AC_BasicCharacter* OwnerCharacter;
};
