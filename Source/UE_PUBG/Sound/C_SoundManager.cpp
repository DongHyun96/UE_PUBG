// Fill out your copyright notice in the Description page of Project Settings.


#include "Sound/C_SoundManager.h"
#include "C_SoundManager.h"
#include "Sound/SoundClass.h"

AC_SoundManager::AC_SoundManager()
{
	PrimaryActorTick.bCanEverTick = true;
	InitializeSoundClassData();

}

void AC_SoundManager::BeginPlay()
{
	Super::BeginPlay();
	InitializeSoundClassData();
}

void AC_SoundManager::InitializeSoundClassData()
{
	static const FString ContextString(TEXT("SoundManager : SoundClass Lookup"));
	SoundClassDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Project_PUBG/Common/Sounds/Sound_Class/DT_SoundClassTable.DT_SoundClassTable"));

	if (SoundClassDataTable)
	{
		//SoundClasstable의 rowName을 SoundManager로 했기 떄문에 매개변수에 SoundManager를 넣어주었음.
		FSoundClassTable* ItemData = SoundClassDataTable->FindRow<FSoundClassTable>(TEXT("SoundManager"), ContextString);
		if (ItemData)
		{
			SoundClassData = *ItemData; 
		}
	}
}

void AC_SoundManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AC_SoundManager::SetVolumeByClass(USoundClass* SoundClass, float Volume)
{
	if (SoundClass)
    {
        SoundClass->Properties.Volume = FMath::Clamp(Volume, 0.f, 1.f);
    }
}

float AC_SoundManager::GetVolumeByClass(USoundClass* SoundClass) const
{
	return SoundClass ? SoundClass->Properties.Volume : 0.f;
}

void AC_SoundManager::SaveVolumeSettings()
{
	
}

void AC_SoundManager::LoadVolumeSettings()
{
}

