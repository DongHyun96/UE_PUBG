// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/Gun/C_AR.h"
#include "Item/Attachment/C_AttachableItem.h"

AC_AR::AC_AR()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AC_AR::BeginPlay()
{
	Super::BeginPlay();

}

void AC_AR::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
