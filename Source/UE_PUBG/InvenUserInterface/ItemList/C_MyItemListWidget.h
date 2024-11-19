// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InvenUserInterface/C_BasicUserWidget.h"
#include "Components/ListView.h"
#include "UE_PUBG/Character/C_BasicCharacter.h"

#include "C_MyItemListWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_MyItemListWidget : public UC_BasicUserWidget
{
	GENERATED_BODY()

protected:
    void NativeConstruct() override;

public:
    UFUNCTION(BlueprintCallable)
    void AddTMapItem(TMap<FString, AC_Item*> itemlist);

    // 아이템 바 위젯을 생성하고 리스트에 추가하는 함수
    UFUNCTION(BlueprintCallable)
    void InitAroundItemList(const TArray<AC_Item*>& AroundItemList);

    UFUNCTION(BlueprintCallable)
    void InitMyItemList(const TMap<FString, AC_Item*> MyItemlist);

public:

    //UPROPERTY(BlueprintReadWrite, EditAnywhere)

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    UListView* ItemListView = nullptr;
};
