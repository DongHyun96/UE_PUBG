// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "C_GameInstance.generated.h"

UENUM(BlueprintType)
enum class EDataTableType: uint8
{
	None,
	BulletImpactSound,
	Gun,
	GunSound,
	HealItemUsingSound,
	Item,
	MeleeSound,
	ThrowingWeaponSound,
	SoundClasses,
	RandomName,
	MAX
};

UENUM(BlueprintType)
enum class ELevelType : uint8
{
	QuickStart,
	ShantyTown,
	TrainingGround,
	Max
};

/// <summary>
/// ELevelType에 대한 Iterator 구현 연산자 오버로딩 (Prefix (++Type))
/// </summary>
/// <param name="Type"></param>
/// <returns></returns>
inline ELevelType& operator++(ELevelType& Type)
{
	uint8 TypeToInt = static_cast<uint8>(Type);
	uint8 Max       = static_cast<uint8>(ELevelType::Max);

	Type = (TypeToInt >= Max - 1) ? static_cast<ELevelType>(0) :
									static_cast<ELevelType>(static_cast<uint8>(Type) + 1);
	return Type;	
}

inline ELevelType& operator--(ELevelType& Type)
{
	uint8 TypeToInt = static_cast<uint8>(Type);
	
	Type = (TypeToInt == 0) ?	static_cast<ELevelType>(static_cast<uint8>(ELevelType::Max) - 1) :
								static_cast<ELevelType>(static_cast<uint8>(Type) - 1);
	return Type;
}


/**
 * 게임 실행 -> 종료 까지 지속적으로 살아있는 Singleton 클래스 (GameManager)
 */
UCLASS()
class UE_PUBG_API UC_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	/// <summary>
	/// <para> Override the Init function from UGameInstance </para>
	/// <para> Called When the GameInstance is initialized </para>
	/// </summary>
	virtual void Init() override;

	/// <summary>
	/// <para> UFUNCTION() required for binding to the PreLoadMap delegate </para>
	/// <para> Called before a new map starts loading </para>
	/// </summary>
	/// <param name="InMapName"> : The name/path of the map that's about to be loaded </param>
	UFUNCTION()
	virtual void BeginLoadingScreen(const FString& InMapName);

	/// <summary>
	/// <para> UFUNCTION() required for binding to the PostLoadMapWithWorld delegate </para>
	/// <para> Called after a map has finished loading </para>
	/// </summary>
 	/// <param name="InLoadedWorld"> : Pointer to the newly loaded world/level </param>
	UFUNCTION()
	virtual void EndLoadingScreen(UWorld* InLoadedWorld);

	UFUNCTION(BlueprintCallable)
	TMap<EDataTableType, UDataTable*> GetDataTables() const { return DataTables; }

	//UFUNCTION(BlueprintCallable)
	//FGameDataTables* GetGameDataTables() const { return GameDataTables; }

	void SetCurrentSelectedLevelType(ELevelType InLevelType) {CurrentSelectedLevelType = InLevelType; }
	ELevelType GetCurrentSelectedLevelType() const { return CurrentSelectedLevelType; }

	/// <summary>
	/// Set CurrentLevelType to next Level Type
	/// </summary>
	void IncreaseCurrentSelectedLevelType() { ++CurrentSelectedLevelType; }

	/// <summary>
	/// Set CurrentLevelType to previousS Level Type
	/// </summary>
	void DecreaseCurrentSelectedLevelType() { --CurrentSelectedLevelType; }

protected:
	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//FGameDataTables* GameDataTables{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<EDataTableType, UDataTable*> DataTables{};
	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//UDataTable* DT_BulletImpactSound = nullptr;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//UDataTable* DT_Gun = nullptr;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//UDataTable* DT_GunSound = nullptr;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//UDataTable* DT_HealItemUsingSound = nullptr;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//UDataTable* DT_Item = nullptr;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//UDataTable* DT_MeleeSound = nullptr;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//UDataTable* DT_ThrowingWeaponSound = nullptr;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//UDataTable* DT_SoundClasses = nullptr;

protected:

	UPROPERTY(BlueprintReadOnly)
	ELevelType CurrentSelectedLevelType{};
	
};
