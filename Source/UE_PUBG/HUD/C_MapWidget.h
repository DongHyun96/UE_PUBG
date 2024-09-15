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
	/// �������Ʈ �ʿ��� ������ Event Function (Map �̹����� MainCircle Material ���� information ����)
	/// </summary>
	/// <param name="PosU"> : ������ U��ǥ </param>
	/// <param name="PosV"> : ������ V��ǥ </param>
	/// <param name="UVRad"> : ������ Radius(UV��ǥ�� ����) </param>
	UFUNCTION(BlueprintImplementableEvent)
	void SetMainCircleInfo(float PosU, float PosV, float UVRad);

	/// <summary>
	/// �������Ʈ �ʿ��� ������ Event Function (Map �̹����� NextCircle Material ���� information ����)
	/// </summary>
	/// <param name="PosU"> : ������ U��ǥ </param>
	/// <param name="PosV"> : ������ V��ǥ </param>
	/// <param name="UVRad"> : ������ Radius(UV��ǥ�� ����) </param>
	UFUNCTION(BlueprintImplementableEvent)
	void SetNextCircleInfo(float PosU, float PosV, float UVRad);

	
};
