// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "C_ANCharacterSound.generated.h"

/**
 * UE_PUBG Project 캐릭터 전용 PlaySound AnimNotify
 * 가장 일반적인 Sound play Anim Notify 상황 처리
 */
UCLASS(BlueprintType)
class UE_PUBG_API UC_ANCharacterSound : public UAnimNotify
{
	GENERATED_BODY()

public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="AnimNotify")
	USoundBase* Sound{};
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="AnimNotify")
	float VolumeMultiplier = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="AnimNotify")
	float PitchMultiplier = 1.f;

	// Enemy는 제외한 Player만 Sound를 재생할지 결정 (Default -> 둘 다 재생)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="AnimNotify")
	bool bOnlyPlayPlayerSound{};		
};
