// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_MapWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_MapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	/// <summary>
	/// 블루프린트 쪽에서 구현한 Event Function (Map 이미지의 MainCircle Material 관련 information 조정)
	/// </summary>
	/// <param name="PosU"> : 중점의 U좌표 </param>
	/// <param name="PosV"> : 중점의 V좌표 </param>
	/// <param name="UVRad"> : 중점의 Radius(UV좌표계 단위) </param>
	UFUNCTION(BlueprintImplementableEvent)
	void SetMainCircleInfo(float PosU, float PosV, float UVRad);

	/// <summary>
	/// 블루프린트 쪽에서 구현한 Event Function (Map 이미지의 NextCircle Material 관련 information 조정)
	/// </summary>
	/// <param name="PosU"> : 중점의 U좌표 </param>
	/// <param name="PosV"> : 중점의 V좌표 </param>
	/// <param name="UVRad"> : 중점의 Radius(UV좌표계 단위) </param>
	UFUNCTION(BlueprintImplementableEvent)
	void SetNextCircleInfo(float PosU, float PosV, float UVRad);

	
};
