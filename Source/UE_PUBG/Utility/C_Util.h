﻿// Fill out your copyright notice in the Description page of Project Settings.

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

	static void Print(const FString&		str,	const FColor& InColor = FColor::Red, const float& TimeToDisplay = 1.f);
	static void Print(const int&			data,	const FColor& InColor = FColor::Red, const float& TimeToDisplay = 1.f);
	static void Print(const float&			data,	const FColor& InColor = FColor::Red, const float& TimeToDisplay = 1.f);
	static void Print(const double&			data,	const FColor& InColor = FColor::Red, const float& TimeToDisplay = 1.f);
	static void Print(const FVector&		data,	const FColor& InColor = FColor::Red, const float& TimeToDisplay = 1.f);
	static void Print(const FVector2D&		data,	const FColor& InColor = FColor::Red, const float& TimeToDisplay = 1.f);
	static void Print(const FTransform&		data,	const FColor& InColor = FColor::Red, const float& TimeToDisplay = 1.f);
	static void Print(const FRotator&		data,	const FColor& InColor = FColor::Red, const float& TimeToDisplay = 1.f);
	static void Print(AActor*				Actor,	const FColor& InColor = FColor::Red, const float& TimeToDisplay = 1.f);

	static void PrintLogMessage(const FString& str);

	static void ClearAllMessages() { GEngine->ClearOnScreenDebugMessages(); }

public:
	/// <summary>
	/// FVector XY 성분 떼어서 FVector2D return
	/// </summary>
	static FVector2D GetXY(const FVector& InVector) { return FVector2D(InVector.X, InVector.Y); }

public:
	
	/// <summary>
	/// TargetUserWidget 내에서 만든 Widget Animation 이름으로 객체 찾기 
	/// </summary>
	/// <param name="TargetUserWidget"> : 해당 Widget Animation을 찾을 UserWidget </param>
	/// <param name="AnimationName"> : Widget Animation name </param>
	/// <returns> : 만약에 찾지 못했다면 return nullptr </returns>
	static class UWidgetAnimation* GetWidgetAnimationByName(UUserWidget* TargetUserWidget, const FName& AnimationName);
	
};


