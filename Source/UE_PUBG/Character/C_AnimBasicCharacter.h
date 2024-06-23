// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "C_AnimBasicCharacter.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_AnimBasicCharacter : public UAnimInstance
{
	GENERATED_BODY()

	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

	
};
