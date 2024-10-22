// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Components/TimelineComponent.h"

#include "C_TimelineUtility.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ECurveType : uint8
{
	CURVEFLOAT,
	CURVEVECTOR,
	CURVELINEARCOLOR,
	TYPE_MAX
};

UCLASS()
class UE_PUBG_API UC_TimelineUtility : public UActorComponent
{
	GENERATED_BODY()

public:
	UC_TimelineUtility();
protected:


public:

	void InitializeTimelines
	(	
		class AC_BasicCharacter* InOwnerCharacter, 
		const TCHAR* FilePath,
		ECurveType InType,
		FName InInterpFunction,
		FName InFinishedFunction,
		float InPlayRate,	
		float InTimelineLength,
		bool InbLoop = false
	);

	void SetTimeLinesCurve
	(
		ECurveType InType,
		FName InInterpFunction,
		FName InFinishedFunction,
		float InPlayRate,
		float InTimelineLength,
		bool InbLoop = false
	);

	void SetTimelinePlayRate(float InPlayrate);
	virtual void PlayFromStart();
protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UTimelineComponent* Timeline;

	//TimeLine 진행중일 때 호출 되는 함수포인터
	FOnTimelineFloat InterpFunctionFloat{};
	FOnTimelineVector InterpFunctionVector{};
	FOnTimelineLinearColor InterpFunctionColor{};
	//TimeLine 이 끝나면 호출 되는 함수포인터
	FOnTimelineEvent TimelineFinished{};


	//FVector InitialCameraLocation;
	//FRotator InitialCameraRotation;

	//void SetTimeLineComponentForMovingCamera();

	class AC_BasicCharacter* OwnerCharter;

	UPROPERTY(EditDefaultsOnly)
	class UCurveFloat* CurveFloat{};
	UPROPERTY(EditDefaultsOnly)
	class UCurveVector* CurveVector{};
	UPROPERTY(EditDefaultsOnly)
	class UCurveLinearColor* CurveColor{};
	
	ECurveType CurType{};
};
