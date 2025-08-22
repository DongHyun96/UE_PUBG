// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_CharacterPreviewWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_CharacterPreviewWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	void SetPreviewCharacter(class AC_PreviewCharacter* InPreviewCharacter) { PreviewCharacter = InPreviewCharacter; }
protected:
	UPROPERTY(BlueprintReadWrite)
	class AC_PreviewCharacter* PreviewCharacter = nullptr;

};
