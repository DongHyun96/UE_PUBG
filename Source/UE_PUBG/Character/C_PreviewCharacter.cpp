// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_PreviewCharacter.h"

// Sets default values
AC_PreviewCharacter::AC_PreviewCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AC_PreviewCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AC_PreviewCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AC_PreviewCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

