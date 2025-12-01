// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/AttachmentActors/AttachmentActor.h"

#include "Item/Weapon/Gun/C_Gun.h"

// Sets default values
AAttachmentActor::AAttachmentActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	//RootComponent = AttachmentMesh;
}

// Called when the game starts or when spawned
void AAttachmentActor::BeginPlay()
{
	Super::BeginPlay();


}

// Called every frame
void AAttachmentActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

bool AAttachmentActor::UseAttachStrategy()
{
	if (!GetAttachParentActor()) return false;
	CurrentGun = Cast<AC_Gun>(GetAttachParentActor());
	if (!IsValid(CurrentGun)) return false;
		
	return true;
}

bool AAttachmentActor::UseDetachStrategy()
{
	if (!GetAttachParentActor()) return false;
	CurrentGun = Cast<AC_Gun>(GetAttachParentActor());
	if (!IsValid(CurrentGun)) return false;
		
	return true;
}



