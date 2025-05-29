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
	FOOT_STEP,
	AIRPLANE,
	LANDING,
	PARACHUTE,
	BGM,
	MAX
};

USTRUCT()
struct FSoundClassVolumeData
{
	GENERATED_BODY()

	UPROPERTY()
	FString SoundClassName{};  // enum이 아닌 string으로 저장 (ex. "BGM", "GUN")

	UPROPERTY()
	float Volume{};  // 볼륨 값 (0.0f ~ 1.0f)

	// 생성자
	FSoundClassVolumeData() : SoundClassName(TEXT("")), Volume(1.f) {}
	FSoundClassVolumeData(FString InName, float InVolume) : SoundClassName(InName), Volume(InVolume) {}
};

USTRUCT(BlueprintType)
struct FSoundClassEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESoundClassName SoundClassName = ESoundClassName::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundClass* SoundClass = nullptr;
};

USTRUCT(BlueprintType)
struct FSoundClassTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSoundClassEntry> SoundClasses;
};

USTRUCT(BlueprintType)
struct FSoundEffectData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Sound = nullptr;  

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESoundClassName SoundClassType = ESoundClassName::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIs2D = true;
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
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	/// <summary>
	/// SoundClassData를 설정해주는 함수.
	/// </summary>
	void InitializeSoundClassData();

	void BuildSoundClassMap(FSoundClassTable* InDataTable);



public:
	UFUNCTION(BlueprintCallable)
	void PlaySoundEffect(ESoundClassName EffectType, FVector Location = FVector::ZeroVector, float InVolumeMultiplier = 1.0f, bool bForce2D = false);

	// 이름으로 사운드 클래스를 찾고 볼륨 조절.
	UFUNCTION(BlueprintCallable)
	void SetVolumeByName(ESoundClassName SoundClassName, float Volume);

	// 이름으로 사운드 클래스를 찾고 볼륨을 가져옴.
	UFUNCTION(BlueprintCallable)
	float GetVolumeByName(ESoundClassName SoundClassName) const;

	// 현재 볼륨	설정을 JSON 파일로 저장.
	UFUNCTION(BlueprintCallable)
	void SaveVolumeSettings();

	// JSON 파일에서 볼륨 설정을 불러옴.
	UFUNCTION(BlueprintCallable)
	void LoadVolumeSettings();

	UFUNCTION(BlueprintCallable)
	USoundClass* GetSoundClassByName(ESoundClassName SoundClassName) 
	{
		if (USoundClass** Found = SoundClassMap.Find(SoundClassName))
		{
			return *Found;
		}

		UE_LOG(LogTemp, Warning, TEXT("SoundClass not found for key: %d"), static_cast<int32>(SoundClassName));
		return nullptr;
	}

public:
	UAudioComponent* GetBGMComponent() const { return BGMComponent; }
protected:
	UPROPERTY()
	TMap<ESoundClassName, USoundClass*> SoundClassMap{};

	UPROPERTY(EditAnywhere, Category = "Sound")
	TMap<ESoundClassName, FSoundEffectData> SoundEffectMap;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<UAudioComponent*> AudioComponents{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAudioComponent* BGMComponent = nullptr;
};
