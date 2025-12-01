// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/C_BasicCharacter.h"
#include "Engine/GameInstance.h"
#include "UE_PUBG/Utility/CommonTypes.h"	
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
	Max,
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

	/// <summary>	/// <para> UFUNCTION() required for binding to the PreLoadMap delegate </para>
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

	const FCharacterSounds* GetCharacterSounds() const { return &CharacterSounds; }

public:

	UFUNCTION(BlueprintCallable)
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

public:

	void SetPlayerNickName(const FString& InNickName);
	
	const FString& GetPlayerNickName() const { return PlayerNickName; }

	bool GetPlayerNickNameSet() const { return bPlayerNickNameSet; }

	UFUNCTION(BlueprintCallable)
	class UInputMappingContext* GetDefaultInputMappingContext() const { return DefaultInputMappingContext; }

	/// <summary>
	/// CacehdItemData에서 ItemRowName에 해당하는 FItemData 구조체를 찾아 반환.
	/// 읽기전용
	/// </summary>
	/// <param name="ItemRowName"></param>
	/// <returns></returns>
	const FItemData* GetItemData(FName ItemRowName) const;
protected:
	/// <summary>
	/// 멤버 변수 CachedItemData를 초기화.
	/// </summary>
	void InitItemDataCache();
protected:

	// 가장 첫 로그인 때 정한 NickName (한 번 정한 이후 변하지 않을 예정)
	UPROPERTY(BlueprintReadOnly)
	FString PlayerNickName = "Dongman"; // 임시 이름으로 일단 지정해놓음 (Log-in 시에 바뀔 예정)
	bool bPlayerNickNameSet{};

protected:
	/// <summary>
	/// 게임에 사용되는 DataTable들을 모아놓은 맵
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<EDataTableType, UDataTable*> DataTables{};

	/// <summary>
	/// 아이템 데이터들을 캐싱해놓은 맵
	/// </summary>
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FName, FItemData> CachedItemData{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputMappingContext* DefaultInputMappingContext{};

protected: // Sound 관련

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FCharacterSounds CharacterSounds{};

protected:

	UPROPERTY(BlueprintReadOnly)
	ELevelType CurrentSelectedLevelType{};
	
};
