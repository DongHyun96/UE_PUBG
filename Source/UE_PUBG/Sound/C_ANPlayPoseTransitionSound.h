// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "C_ANPlayPoseTransitionSound.generated.h"

/**
 * 이 클래스를 기반으로 하는 4개의 PoseTransition AN 블루프린트 생성해서 해당 sound를 Transition에 따라 초기화 시켜줄 예정
 */
UCLASS()
class UE_PUBG_API UC_ANPlayPoseTransitionSound : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	USoundBase* PoseTransitionSound{};
	
};
