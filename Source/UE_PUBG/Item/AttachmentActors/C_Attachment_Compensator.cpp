// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/AttachmentActors/C_Attachment_Compensator.h"
#include "Components/SkeletalMeshComponent.h"
#include "Item/Weapon/Gun/C_Gun.h"


AC_Attachment_Compensator::AC_Attachment_Compensator()
{
	PrimaryActorTick.bCanEverTick = true;
	PartName	   = EPartsName::MUZZLE;
	AttachmentName = EAttachmentNames::COMPENSATOR;
	AttachmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMesh");
}

void AC_Attachment_Compensator::BeginPlay()
{
	Super::BeginPlay();
}

void AC_Attachment_Compensator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AC_Attachment_Compensator::UseAttachStrategy()
{
	if (!GetAttachParentActor())
		return false;
	AC_Gun* CurrentGun = Cast<AC_Gun>(GetAttachParentActor());
	if (!IsValid(CurrentGun))
		return false;
	CurrentGun->SetRecoilMultiplierMuzzleHorizon(0.9f);
	CurrentGun->SetRecoilMultiplierMuzzleVert(0.9f);
	return true;
}

bool AC_Attachment_Compensator::UseDetachStrategy()
{
	if (!GetAttachParentActor())
		return false;
	AC_Gun* CurrentGun = Cast<AC_Gun>(GetAttachParentActor());
	if (!IsValid(CurrentGun))
		return false;
	CurrentGun->SetRecoilMultiplierMuzzleHorizon(1.0f);
	CurrentGun->SetRecoilMultiplierMuzzleVert(1.0f);
	DetachRootComponentFromParent();
	SetActorHiddenInGame(true);
	return true;
}

bool AC_Attachment_Compensator::UseMrbStrategy()
{
	return false;
}
