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
//
//public:
//
//	// 초기화 (복사본 생성 및 적용)
//	void InitializeInputContext();
//
//	// 키를 교체 
//	void SetKeyForAction(FName ActionName, const FKey& MainKey, const FKey& SubKey);
//
//	// 현재 키를 SaveGame으로 저장 
//	void SaveKeyMappings();
//
//	// 저장된 키 불러오기 
//	void LoadKeyMappings();
//
//	// 기본키로 되돌리기 
//	void ResetToDefault();
//
//	//새로운 액션 추가시, 기존 커스텀에 자동 반영 
//	void SyncWithDefaultIMC();
//
//	const FEnhancedActionKeyMapping* FindInputActionByName(FName ActionName);

protected:
    //void ApplyDefaultMappings();               // IMC_Default → UserIMC
    //void EnsureNewActionsArePatched();         // 신규 InputAction 자동 반영

	// 원본 InputContext, 에디터에서 정의
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputMappingContext* IMC_Player = nullptr;

	// 유저 커스텀용 InputContext (런타임에 복제됨) 
	UPROPERTY()
	UInputMappingContext* IMC_User = nullptr;

	// 유저가 변경한 키값을 저장하는 맵 
	UPROPERTY()
	TMap<FName, FInputKeyData> SavedKeyMappings;


};
