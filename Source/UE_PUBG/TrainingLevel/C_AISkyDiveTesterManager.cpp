// Fill out your copyright notice in the Description page of Project Settings.


#include "C_AISkyDiveTesterManager.h"

#include "Components/BoxComponent.h"
#include "Utility/C_Util.h"


AC_AISkyDiveTesterManager::AC_AISkyDiveTesterManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_AISkyDiveTesterManager::BeginPlay()
{
	Super::BeginPlay();

	SkyDiveStartAreaBoxComponent   = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("SkyDiveStartArea")));
	SkyDiveLandingAreaBoxComponent = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("SkyDiveLandingArea")));

	if (!SkyDiveStartAreaBoxComponent)
		UC_Util::Print("From AC_AISkyDiveTesterManager::BeginPlay : SkyDiveStart Area Component not initialized!", FColor::Red, 10.f);

	if (!SkyDiveLandingAreaBoxComponent)
		UC_Util::Print("From AC_AISkyDiveTesterManager::BeginPlay : SkyDiveLanding Area Component not initialized!", FColor::Red, 10.f);
}

void AC_AISkyDiveTesterManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector AC_AISkyDiveTesterManager::GetRandomLocationInBox(UBoxComponent* Box) const
{
	if (!Box)
	{
		UC_Util::Print("From AC_AISKyDiveTesterManager::GetRandomLocationInBox : Input box not valid!", FColor::Red, 10.f);
		return FVector::ZeroVector;
	}

	const FVector Origin = Box->GetComponentLocation();
	const FVector Extent = Box->GetScaledBoxExtent();

	const float RandX = FMath::FRandRange(-Extent.X, Extent.X);
	const float RandY = FMath::FRandRange(-Extent.Y, Extent.Y);
	const float RandZ = FMath::FRandRange(-Extent.Z, Extent.Z);

	// 월드 좌표로 변환
	return Origin + FVector(RandX, RandY, RandZ);
}

