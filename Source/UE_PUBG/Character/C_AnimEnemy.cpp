// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_AnimEnemy.h"

void UC_AnimEnemy::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwnerCharacter) return;

	Speed = OwnerCharacter->GetVelocity().Size2D();
}


