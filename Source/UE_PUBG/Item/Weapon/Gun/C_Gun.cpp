// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Weapon/Gun/C_Gun.h"
#include "Character/C_BasicCharacter.h"
#include "Character/Component/C_EquippedComponent.h"


// Sets default values
AC_Gun::AC_Gun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AC_Gun::BeginPlay()
{
	Super::BeginPlay();

}

void AC_Gun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AC_Gun::AttachToHolster(USceneComponent* InParent)
{
	return AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
		HOLSTER_SOCKET_NAME
	);

}

bool AC_Gun::AttachToHand(USceneComponent* InParent)
{
	OwnerCharacter->SetHandState(EHandState::WEAWPON_GUN);

	return AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
		EQUIPPED_SOCKET_NAME
	);

}


