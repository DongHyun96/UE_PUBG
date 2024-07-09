// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equipment/C_BackPack.h"

AC_BackPack::AC_BackPack()
{
	PrimaryActorTick.bCanEverTick = true;

	Level = 2;
}

void AC_BackPack::Interaction(AC_BasicCharacter* character)
{
	AC_BasicCharacter* testCharacter = character;


}
