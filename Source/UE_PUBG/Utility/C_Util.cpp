// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/C_Util.h"

void UC_Util::Print(const FString& str, const FColor& InColor)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0, InColor, *str);
}

void UC_Util::Print(const int& data, const FColor& InColor)
{
	FString str = FString::FromInt(data);
	GEngine->AddOnScreenDebugMessage(-1, 1.0, InColor, *str);
}

void UC_Util::Print(const float& data, const FColor& InColor)
{
	FString str = FString::SanitizeFloat(data);
	GEngine->AddOnScreenDebugMessage(-1, 1.0, InColor, *str);
}

void UC_Util::Print(const FVector& data, const FColor& InColor)
{
	FString str = data.ToString();
	GEngine->AddOnScreenDebugMessage(-1, 1.0, InColor, *str);
}

void UC_Util::Print(const FTransform& data, const FColor& InColor)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0, InColor, *data.ToString());
}