// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "C_FootStepFallingData.generated.h"

/**
 *  AnimNotify로 FootStep 처리 시, Falling 착지 Animation을 구분하기 위한 AssetUserData
 *  Falling 착지 Animation의 AssetUserData에 추가하여 태그처럼 쓰일 예정
 */
UCLASS()
class UE_PUBG_API UC_FootStepFallingData : public UAssetUserData
{
	GENERATED_BODY()
};
