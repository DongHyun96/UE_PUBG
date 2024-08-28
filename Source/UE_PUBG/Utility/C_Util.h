// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "C_Util.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_Util : public UObject
{
	GENERATED_BODY()
	
public:

	static void Print(const FString& str, const FColor& InColor = FColor::Red, const float& TimeToDisplay = 1.f);
	static void Print(const int& data, const FColor& InColor = FColor::Red, const float& TimeToDisplay = 1.f);
	static void Print(const float& data, const FColor& InColor = FColor::Red, const float& TimeToDisplay = 1.f);
	static void Print(const FVector& data, const FColor& InColor = FColor::Red, const float& TimeToDisplay = 1.f);
	static void Print(const FTransform& data, const FColor& InColor = FColor::Red, const float& TimeToDisplay = 1.f);
	static void Print(const FRotator& data, const FColor& InColor = FColor::Red, const float& TimeToDisplay = 1.f);

};
