// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "UE_PUBG/Character/C_BasicCharacter.h"
#include "C_MyItemListWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_MyItemListWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    void NativeConstruct() override;

public:
    UFUNCTION(BlueprintCallable)
    void AddTMapItem(TMap<FString, AC_Item*> itemlist);

    // ������ �� ������ �����ϰ� ����Ʈ�� �߰��ϴ� �Լ�
    UFUNCTION(BlueprintCallable)
    void PopulateItemList(const TMap<FString, AC_Item*>& itemList);

public:

    //UPROPERTY(BlueprintReadWrite, EditAnywhere)

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    UListView* ItemListBar = nullptr;



protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
    class AC_BasicCharacter* OwnerCharacter;
};
