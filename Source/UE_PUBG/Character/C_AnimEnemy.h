// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/C_AnimBasicCharacter.h"
#include "C_AnimEnemy.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_AnimEnemy : public UC_AnimBasicCharacter
{
	GENERATED_BODY()

private:

	/// <summary>
	/// AnimGraph에서의 transition용 data들 업데이트
	/// </summary>
	void NativeUpdateAnimation(float DeltaSeconds) override;
	
};
