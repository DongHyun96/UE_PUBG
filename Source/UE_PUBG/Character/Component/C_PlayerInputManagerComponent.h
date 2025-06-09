// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_PlayerInputManagerComponent.generated.h"

USTRUCT(BlueprintType)
struct FInputKeyData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName ActionName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FKey MainKey;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FKey SubKey;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_PlayerInputManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UC_PlayerInputManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
    void InitializeInput(class APlayerController* Controller);

    void LoadUserKeySettings();
    void SaveUserKeySettings();
    void ResetToDefaultKeySettings();

    void RebindKey(class UInputAction* Action, FKey NewKey, bool bIsPrimary = true);

protected:
    void ApplyDefaultMappings();               // IMC_Default → UserIMC
    void EnsureNewActionsArePatched();         // 신규 InputAction 자동 반영

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
    class UInputMappingContext* DefaultPlayerInputMappingContext = nullptr;

    UPROPERTY()
    UInputMappingContext* UserMappingContext = nullptr;

    UPROPERTY()
    class APlayerController* CachedController = nullptr;

	/** 유저가 변경한 키값을 저장하는 맵 */
	UPROPERTY()
	TMap<FName, FInputKeyData> SavedKeyMappings;
		
};
