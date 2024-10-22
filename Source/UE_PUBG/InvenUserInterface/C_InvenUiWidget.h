// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"


#include "C_InvenUiWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_InvenUiWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetOwnerCharacter(class AC_BasicCharacter* Character) { OwnerCharacter = Character; }

	void InitListView();
protected:
	virtual void NativeConstruct() override;

	//virtual void PreConstruct() override;

	void PopulateItemList(class UListView* list, const TMap<FString, class AC_Item*>& itemList);

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UCanvasPanel* InvenCanvas = nullptr;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UC_MyItemListWidget* MyItemListWidget = nullptr; // 아이템 리스트 위젯

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UC_MyItemListWidget* AroundItemListWidget = nullptr; // 아이템 리스트 위젯

	class UListView* MyItemListView = nullptr;

	UListView* AroundItemListView = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UC_ItemBarWidget> ItemBarClass;

	UC_ItemBarWidget* ItemBarInstance = nullptr;

	UUserWidget* widget = nullptr;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AC_BasicCharacter* OwnerCharacter = nullptr;
};
