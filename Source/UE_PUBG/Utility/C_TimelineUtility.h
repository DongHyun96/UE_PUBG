// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Components/TimelineComponent.h"

#include "C_TimelineUtility.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_TimelineUtility : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UTimelineComponent* CameraTransitionTimeline;

	//TimeLine �������� �� ȣ�� �Ǵ� �Լ�������
	FOnTimelineFloat InterpFunction{};
	//TimeLine �� ������ ȣ�� �Ǵ� �Լ�������
	FOnTimelineEvent TimelineFinished{};
	UFUNCTION()
	void HandleInterpolation(float Value);

	UFUNCTION()
	void OnTimelineFinished();

	//FVector InitialCameraLocation;
	//FRotator InitialCameraRotation;

	//void SetTimeLineComponentForMovingCamera();



	UPROPERTY(EditDefaultsOnly)
	class UCurveFloat* CurveFloat{};
	UPROPERTY(EditDefaultsOnly)
	class UCurveVector* CurveVector{};
	UPROPERTY(EditDefaultsOnly)
	class UCurveLinearColor* CurveColor{};
	
};
