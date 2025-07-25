// Fill out your copyright notice in the Description page of Project Settings.


#include "Sound/C_SoundManager.h"
#include "C_SoundManager.h"
#include "Sound/SoundClass.h"
#include "Singleton/C_GameInstance.h"
//#include "Misc/FileHelper.h"
//#include "Misc/Paths.h"
//#include "Serialization/JsonWriter.h"
//#include "Serialization/JsonSerializer.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Utility/C_Util.h"

AC_SoundManager::AC_SoundManager()
{
	PrimaryActorTick.bCanEverTick = true;
	//InitializeSoundClassData();

	BGMComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("BGMComponent"));
	BGMComponent->SetupAttachment(RootComponent);

	AudioComponents.Add(BGMComponent);
}

void AC_SoundManager::BeginPlay()
{
	Super::BeginPlay();
	InitializeSoundClassData();
	// 
	//LoadVolumeSettings();
}

void AC_SoundManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AC_SoundManager::InitializeSoundClassData()
{
	static const FString ContextString(TEXT("SoundManager"));

	static const FName SoundManagerRowName(TEXT("SoundManager"));

	UC_GameInstance* GI = Cast<UC_GameInstance>(GetGameInstance());

	UDataTable* SoundClassDataTable = GI->GetDataTables()[EDataTableType::SoundClasses];

	if (SoundClassDataTable)
	{
		FSoundClassTable* Row = SoundClassDataTable->FindRow<FSoundClassTable>(SoundManagerRowName, ContextString);
		if (Row)
		{
			BuildSoundClassMap(Row);
		}
		else
		{
			UC_Util::Print("Failed to find row in SoundClassDataTable");
		}
	}
}

void AC_SoundManager::BuildSoundClassMap(FSoundClassTable* InDataTable)
{
	SoundClassMap.Empty();

	for (const FSoundClassEntry& Entry : InDataTable->SoundClasses)
	{
		if (Entry.SoundClassName != ESoundClassName::NONE && Entry.SoundClassName != ESoundClassName::MAX && Entry.SoundClass)
		{
			SoundClassMap.Add(Entry.SoundClassName, Entry.SoundClass);
		}
	}
}

void AC_SoundManager::PlaySoundEffect(ESoundClassName EffectType, FVector Location, float InVolumeMultiplier, bool bForce2D)
{
	if (!SoundEffectMap.Contains(EffectType)) return;

	const FSoundEffectData& EffectData = SoundEffectMap[EffectType];

	if (!EffectData.Sound) return;

	float FinalVolume = EffectData.VolumeMultiplier * GetVolumeByName(EffectType) * InVolumeMultiplier;

	const bool bUse2D = bForce2D || EffectData.bIs2D;

	if (bUse2D)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), EffectData.Sound, FinalVolume);
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), EffectData.Sound, Location, FinalVolume);
	}
}

void AC_SoundManager::SetVolumeByName(ESoundClassName SoundClassName, float Volume)
{
	float InVolume = 0.01f;

	if (
		SoundClassName == ESoundClassName::MASTER  || 
		SoundClassName == ESoundClassName::GLIDING || 
		SoundClassName == ESoundClassName::AIRPLANE 
		)
	{
		InVolume = Volume ? Volume : 0.01f;

		//if (Volume == 0.f)
		//	InVolume = 0.01f;
		//else
		//	InVolume = Volume;
	}
	else
	{
		InVolume = Volume;
	}

	if (USoundClass** FoundClass = SoundClassMap.Find(SoundClassName))
	{
		(*FoundClass)->Properties.Volume = FMath::Clamp(InVolume, 0.f, 1.f);
	}
}

float AC_SoundManager::GetVolumeByName(ESoundClassName SoundClassName) const
{
	if (const USoundClass* const* FoundClass = SoundClassMap.Find(SoundClassName))
	{
		return (*FoundClass)->Properties.Volume;
	}
	return 0.f;
}

void AC_SoundManager::SaveVolumeSettings()
{
	TArray<TSharedPtr<FJsonValue>> JsonArray;

	for (const TPair<ESoundClassName, USoundClass*>& Pair : SoundClassMap)
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

		// enum → string 변환
		FString ClassName = StaticEnum<ESoundClassName>()->GetNameStringByValue((int64)Pair.Key);

		JsonObject->SetStringField(TEXT("SoundClassName"), ClassName);
		JsonObject->SetNumberField(TEXT("Volume"), Pair.Value->Properties.Volume);

		JsonArray.Add(MakeShareable(new FJsonValueObject(JsonObject)));
	}

	// JSON 직렬화
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonArray, Writer);

	// 저장 경로
	FString SavePath = FPaths::ProjectSavedDir() + "SoundSettings.json";
	FFileHelper::SaveStringToFile(OutputString, *SavePath);
}

void AC_SoundManager::LoadVolumeSettings()
{
	FString LoadPath = FPaths::ProjectSavedDir() + "SoundSettings.json";
	FString JsonStr;

	if (!FFileHelper::LoadFileToString(JsonStr, *LoadPath))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Failed to load sound settings file"));
		UC_Util::Print("Failed to load sound settings file");
		return;
	}

	TSharedPtr<FJsonValue> JsonParsed;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonStr);

	if (!FJsonSerializer::Deserialize(Reader, JsonParsed) || !JsonParsed.IsValid())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Failed to parse JSON"));
		UC_Util::Print("Failed to parse JSON");
		return;
	}

	TArray<TSharedPtr<FJsonValue>> JsonArray = JsonParsed->AsArray();

	for (TSharedPtr<FJsonValue> JsonValue : JsonArray)
	{
		TSharedPtr<FJsonObject> JsonObject = JsonValue->AsObject();
		FString NameStr = JsonObject->GetStringField("SoundClassName");
		float Volume = JsonObject->GetNumberField("Volume");

		// 문자열 → enum 변환
		ESoundClassName SoundClassEnum = ESoundClassName::NONE;
		UEnum* EnumPtr = StaticEnum<ESoundClassName>();
		if (EnumPtr)
		{
			int64 EnumValue = EnumPtr->GetValueByName(FName(NameStr));
			if (EnumValue != INDEX_NONE)
			{
				SoundClassEnum = static_cast<ESoundClassName>(EnumValue);
			}
		}

		if (SoundClassEnum != ESoundClassName::NONE)
		{
			SetVolumeByName(SoundClassEnum, Volume);
		}
	}
}
