﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/AttachmentActors/C_Attachment_RedDot.h"
#include "Item/Weapon/Gun/C_Gun.h"

AC_Attachment_RedDot::AC_Attachment_RedDot()
{
	PrimaryActorTick.bCanEverTick = true;
	PartName = EPartsName::SCOPE;
	AttachmentName = EAttachmentNames::REDDOT;
}

void AC_Attachment_RedDot::BeginPlay()
{
	Super::BeginPlay();

}

void AC_Attachment_RedDot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AC_Attachment_RedDot::UseAttachStrategy()
{
	if (!GetAttachParentActor())
		return false;
	AC_Gun* CurrentGun = Cast<AC_Gun>(GetAttachParentActor());
	if (!IsValid(CurrentGun))
		return false;
	CurrentGun->SetSightCameraSpringArmLocation(CurrentGun->GetScopeCameraLocations()[AttachmentName]);
	CurrentGun->SetScopeCameraMode(AttachmentName);
	CurrentGun->SetIronSightMeshHiddenInGame(true);

	return true;
}

bool AC_Attachment_RedDot::UseDetachStrategy()
{
	if (!GetAttachParentActor())
		return false;
	AC_Gun* CurrentGun = Cast<AC_Gun>(GetAttachParentActor());
	if (!IsValid(CurrentGun))
		return false;
	CurrentGun->SetSightCameraSpringArmLocation(CurrentGun->GetScopeCameraLocations()[EAttachmentNames::MAX]);
	CurrentGun->SetScopeCameraMode(EAttachmentNames::MAX);
	CurrentGun->SetIronSightMeshHiddenInGame(false);
	DetachRootComponentFromParent();
	SetActorHiddenInGame(true);

	return true;
}



