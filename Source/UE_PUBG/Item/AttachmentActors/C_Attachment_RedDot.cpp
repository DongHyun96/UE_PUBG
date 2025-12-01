// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/AttachmentActors/C_Attachment_RedDot.h"
#include "Item/Weapon/Gun/C_Gun.h"

AC_Attachment_RedDot::AC_Attachment_RedDot()
{
	PrimaryActorTick.bCanEverTick = false;
	PartName = EPartsName::SCOPE;
	AttachmentName = EAttachmentNames::REDDOT;
	//RootComponent = AttachmentMesh;
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
	if (!Super::UseAttachStrategy()) return false;
	
	CurrentGun->SetSightCameraSpringArmLocation(CurrentGun->GetScopeCameraLocations()[AttachmentName]);
	CurrentGun->SetScopeCameraMode(AttachmentName);
	CurrentGun->SetIronSightMeshHiddenInGame(true);

	return true;
}

bool AC_Attachment_RedDot::UseDetachStrategy()
{
	if (!Super::UseDetachStrategy()) return false;
	
	CurrentGun->SetSightCameraSpringArmLocation(CurrentGun->GetScopeCameraLocations()[EAttachmentNames::MAX]);
	CurrentGun->SetScopeCameraMode(EAttachmentNames::MAX);
	CurrentGun->SetIronSightMeshHiddenInGame(false);
	
	DetachRootComponentFromParent();
	
	SetActorHiddenInGame(true);

	return true;
}
