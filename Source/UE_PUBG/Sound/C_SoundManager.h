// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_SoundManager.generated.h"

UENUM(BlueprintType)
enum class EPreferences : uint8
{
	GRAPHIC,
	AUDIO,
	CONTROL,
	KEY,
	GAMEPLAY,
	MAX
};

UENUM(BlueprintType)
enum class ESoundClassName : uint8
{
	NONE,
	MASTER,
	BULLET_IMPACT,
	GUN,
	USING_HEAL_ITEM,
	MELEE_WEAPON,
	THROWING_WEAPON,
	MAX
};

USTRUCT(BlueprintType)
struct FSoundClassTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundClass* MasterSoundClass = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundClass* BulletImpactSoundClass = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundClass* GunSoundClass = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundClass* UsingHealItemSoundClass = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundClass* MeleeWeaponSoundClass = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundClass* ThrowingWeaponSoundClass = nullptr;
};

UCLASS()
class UE_PUBG_API AC_SoundManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AC_SoundManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	/// <summary>
	/// SoundClassData를 설정해주는 함수.
	/// </summary>
	void InitializeSoundClassData();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/// <summary>
	/// SoundClassData내의 Soundclass의 볼륨을 설정해주는 함수.
	/// </summary>
	/// <param name="SoundClass">조정하고자 하는 Sound</param>
	/// <param name="Volume">사운드 조정값</param>
	UFUNCTION(BlueprintCallable)
	void SetVolumeByClass(USoundClass* SoundClass, float Volume);

	/// <summary>
	/// SoundClassData내의 Soundclass의 볼륨 크기를 가져오는 함수
	/// </summary>
	/// <param name="SoundClass">가져오고자 하는 사운드</param>
	/// <returns>볼륨 크기</returns>
	UFUNCTION(BlueprintCallable)
	float GetVolumeByClass(USoundClass* SoundClass) const;

	/// <summary>
	/// 사운드 조정값을 저장하는 함수.
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void SaveVolumeSettings();

	/// <summary>
	/// 저장된 사운드 조정값을 불러오는 함수.
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void LoadVolumeSettings();


	UFUNCTION(BlueprintCallable)
	FSoundClassTable GetSoundClassData() const { return SoundClassData; }
protected:

	FSoundClassTable SoundClassData{};

	UPROPERTY(EditAnywhere)
	UDataTable* SoundClassDataTable = nullptr;
};
