// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/C_AnimBasicCharacter.h"
#include "C_AnimPlayer.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_AnimPlayer : public UC_AnimBasicCharacter
{
	GENERATED_BODY()

	virtual void NativeBeginPlay() override;

private:

	void SetAimOffsetRotation() override;

	void AnimNotify_OnFallingHardToHardLanding() override;

	void AnimNotify_OnAnyFallingOrJumpingStateToStand() override;

protected:

	// Stand To Jump or falling transition callback 
	UFUNCTION(BlueprintCallable)
	void AnimNotify_OnStandToJumpOrFalling();

	// FallingIdle to FallingHard Transition callback
	UFUNCTION(BlueprintCallable)
	void AnimNotify_OnFallingIdleToFallingHard();
	
private:

	AC_Player* OwnerPlayer{};
	
};


