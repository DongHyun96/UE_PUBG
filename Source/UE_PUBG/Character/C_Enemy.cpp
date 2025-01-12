// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"

AC_Enemy::AC_Enemy()
{
	TeamID = ETeamAttitude::Hostile;
}

void AC_Enemy::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	
}

