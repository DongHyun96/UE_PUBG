// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Equipment/C_BackPack.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Character/Component/C_InvenComponent.h"

AC_BackPack::AC_BackPack()
{
	PrimaryActorTick.bCanEverTick = true;

	Level = 2;
}

void AC_BackPack::Interaction(AC_BasicCharacter* character)
{
	character->GetInvenComponent()->ChackMyBackPack(this);
}
