// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "C_AnimNotify_FootStep.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_AnimNotify_FootStep : public UAnimNotify
{
	GENERATED_BODY()
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FName FootSocketName{};

	UPROPERTY(BlueprintReadOnly)
	FVector SoundLocation{};

	UPROPERTY(BlueprintReadOnly)
	float FootSoundVolume{};

	UPROPERTY(BlueprintReadOnly)
	bool bIsValidToPlaySound{};
};
