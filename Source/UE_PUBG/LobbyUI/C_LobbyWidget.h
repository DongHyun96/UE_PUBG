// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_LobbyWidget.generated.h"

enum class ELevelType : uint8;
/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_LobbyWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;

public:
	
	/// <summary>
	/// <para> Map selection ToggleButtonGroup에서 Map Selected 되었을 때, Callback 받는 함수 </para> 
	/// <para> Map Selection panel의 배경 이미지 및 Animation 재생 & 맵 이름 변경 처리 </para> 
	/// </summary>
	/// <param name="InSelected"></param>
	void OnSelectedMapChanged(ELevelType InSelected);

protected:

	UPROPERTY(meta = (BindWidget))
	class UImage* MapSelectionMapImage{};
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<ELevelType, UTexture2D*> LevelSelectionBackGroundImages{};

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MapSelectedNameText{};

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MapSelectedSizeText{};

	UPROPERTY(meta = (BindWidget))
	class UC_ToggleButtonGroupWidget* MapSelectToggleButtonGroup{};

protected:
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* BackgroundMapImageAnimation{};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* MapNameSizeAnimation{};
};
